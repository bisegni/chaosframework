/*
 *	AbstractRemoteIODriver.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/07/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/cu_toolkit/external_gateway/external_gateway.h>

#include <chaos/common/utility/TimingUtil.h>

#define INFO    INFO_LOG(AbstractCDataWrapperIODriver)
#define DBG		DBG_LOG(AbstractCDataWrapperIODriver)
#define ERR		ERR_LOG(AbstractCDataWrapperIODriver)

#define TIMEOUT_PURGE_PROMISE 60000
#define PURGE_TIMER_REPEAT_DELAY 30000

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

using namespace chaos::cu::external_gateway;
using namespace chaos::cu::driver_manager::driver;

#pragma mark DriverResultInfo
bool AbstractRemoteIODriver::DriverResultInfo::less::operator()(const DriverResultInfo::DriverResultInfoShrdPtr& h1,
                                                                const DriverResultInfo::DriverResultInfoShrdPtr& h2) {
    return h1->request_index < h2->request_index;
}

const AbstractRemoteIODriver::DriverResultInfo::extract_index::result_type&
AbstractRemoteIODriver::DriverResultInfo::extract_index::operator()(const DriverResultInfo::DriverResultInfoShrdPtr &p) const {
    return p->request_index;
}

const AbstractRemoteIODriver::DriverResultInfo::extract_req_ts::result_type&
AbstractRemoteIODriver::DriverResultInfo::extract_req_ts::operator()(const DriverResultInfo::DriverResultInfoShrdPtr &p) const {
    return p->request_ts;
}

#pragma mark AbstractRemoteDriver
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractRemoteIODriver),
ExternalUnitEndpoint(),
authorization_key(),
conn_phase(RDConnectionPhaseDisconnected),
message_counter(0),
set_p_req_id_index(boost::multi_index::get<tag_req_id>(set_p())),
set_p_req_ts_index(boost::multi_index::get<tag_req_ts>(set_p())){}

AbstractRemoteIODriver::~AbstractRemoteIODriver() {}

#pragma mark Public Method
void AbstractRemoteIODriver::driverInit(const char *initParameter) throw(chaos::CException) {
    INFO << "Init driver:"<<initParameter;
    CHECK_ASSERTION_THROW_AND_LOG(isDriverParamInJson(), ERR, -1, "Init parameter need to be formated in a json document");
    
    Json::Value root_param_document = getDriverParamJsonRootElement();
    
    Json::Value jv_endpoint_name = root_param_document["endpoint_name"];
    Json::Value jv_autorization_key = root_param_document["authorization_key"];
    CHECK_ASSERTION_THROW_AND_LOG((jv_endpoint_name.isNull() == false), ERR, -2, "The endpoint name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG((jv_autorization_key.isNull() == false), ERR, -3, "The authorization key is mandatory");
    
    //! end point identifier & authorization key
    ExternalUnitEndpoint::endpoint_identifier = jv_endpoint_name.asString();
    authorization_key = jv_autorization_key.asString();
    CHECK_ASSERTION_THROW_AND_LOG(authorization_key.size(), ERR, -4, "The authorization key cannot be zero lenght");
    
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 PURGE_TIMER_REPEAT_DELAY,
                                                 PURGE_TIMER_REPEAT_DELAY);
    
    //register this driver as external endpoint
    ExternalUnitGateway::getInstance()->registerEndpoint(*this);
}

void AbstractRemoteIODriver::driverDeinit() throw(chaos::CException) {
    INFO << "Deinit driver";
    //registerthis driver as external endpoint
    ExternalUnitGateway::getInstance()->deregisterEndpoint(*this);
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void AbstractRemoteIODriver::handleNewConnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier() = connection_identifier;
    conn_phase = RDConnectionPhaseConnected;
}

void AbstractRemoteIODriver::handleDisconnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier().clear();
    conn_phase = RDConnectionPhaseDisconnected;
}

int AbstractRemoteIODriver::handleReceivedeMessage(const std::string& connection_identifier,
                                                   ChaosUniquePtr<CDataWrapper> message) {
    if(conn_phase != RDConnectionPhaseAutorized) {
        //check if a message with autorization key will arive
        if(message->hasKey("authorization_key") &&
           message->isStringValue("authorization_key")) {
            if(authorization_key.compare(message->getStringValue("authorization_key"))){
                conn_phase = RDConnectionPhaseAutorized;
            }
        }
    } else {
        if(!message->hasKey("request_index")) {
            int err = AR_ERROR_OK;
            MessageType received_message_type;
            // spontaneus message from driver, that can forward error message
            // or read answer for variable change
            if((err = getMessageType(*message,
                                     received_message_type))) {
                ERR << "Error decoding received message type";
                return AR_ERROR_INVALID_MESSAGE_TYPE;
            }
            
            if(received_message_type == MessageTypeVariableRead) {
                //we have te new variable value
                err = updateVariableCachedValue(*message);
            }
        } else if(!message->isInt32Value("request_index")) {
            //send error because not right type of req index
            sendError(connection_identifier,
                      -1, "request_index field need to be a int32 type", __PRETTY_FUNCTION__);
        } else {
            //good request index
            const int64_t req_index = message->getUInt32Value("request_index");
            LSetPromiseWriteLock wl = set_p.getWriteLockObject();
            SetPromisesReqTSIndexIter it = set_p_req_ts_index.find(req_index);
            if(it != set_p_req_ts_index.end()) {
                //set promises and remove it
                (*it)->promise.set_value(CDWShrdPtr(message.release()));
                set_p_req_ts_index.erase(it);
            }
        }
    }
    return 0;
}

void AbstractRemoteIODriver::composeRequest(chaos::common::data::CDWUniquePtr& ext_msg,
                                            uint32_t request_index,
                                            MessageType message_type,
                                            chaos::common::data::CDWUniquePtr& message_data) {
    CHAOS_ASSERT(ext_msg)
    composeMessage(ext_msg,
                   message_type,
                   message_data);
    ext_msg->addInt32Value("request_index", request_index);
}

void AbstractRemoteIODriver::composeMessage(chaos::common::data::CDWUniquePtr& ext_msg,
                                            MessageType message_type,
                                            chaos::common::data::CDWUniquePtr& message_data) {
    CHAOS_ASSERT(ext_msg)
    ext_msg->addInt32Value("type", (int32_t)message_type);
    ext_msg->addCSDataValue("message", *message_data);
}

void AbstractRemoteIODriver::timeout() {
    LSetPromiseWriteLock wl = set_p.getWriteLockObject();
    uint64_t current_check_ts = TimingUtil::getTimeStamp();
    unsigned int max_purge_check = 10;
    for(SetPromisesReqTSIndexIter it = set_p_req_ts_index.begin(),
        end = set_p_req_ts_index.end();
        it != end && max_purge_check;
        max_purge_check--){
        //purge outdated promise
        if(current_check_ts > (*it)->request_ts+TIMEOUT_PURGE_PROMISE) {
            DBG << CHAOS_FORMAT("Remove the pormises for request of index %1%", %(*it)->request_ts);
            set_p_req_ts_index.erase(it++);
        } else {
            ++it;
        }
    }
}

#pragma mark Public API
int AbstractRemoteIODriver::getDeviceList(ChaosStringSet& hosted_device,
                                          uint32_t timeout) {
    int err = AR_ERROR_OK;
    CDWShrdPtr answer;
    CDWUniquePtr ds_req_msg(new CDataWrapper());
    ds_req_msg->addStringValue("mtype", "cat");
    if((err = sendRawRequest(MessageTypeMetadataGetDeviceList,
                             ChaosMoveOperator(ds_req_msg),
                             answer,
                             timeout))) {
        return err;
    }
    if(!answer) {
        return  AR_ERROR_INVALID_MESSAGE_STRUCTURE;
    }
    //we have data
    if(answer->hasKey("device_list") == false ||
       answer->isVectorValue("device_list") == false) {
        return AR_ERROR_INVALID_MESSAGE_STRUCTURE;
    }
    
    //we can get the list
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> arr(answer->getVectorValue("device_list"));
    for(int idx = 0;
        idx < arr->size();
        idx++) {
        if(arr->isStringElementAtIndex(idx) == false) {
            ERR <<"Device element in array is not a string";
            hosted_device.clear();
            return AR_ERROR_INVALID_MESSAGE_STRUCTURE;
        }
        hosted_device.insert(arr->getStringElementAtIndex(idx));
    }
    return err;
}

int AbstractRemoteIODriver::getDeviceDescription(const int32_t& device_index,
                                                 AbstractRemoteIODriver::Device& device,
                                                 bool force_update_cache,
                                                 uint32_t timeout) {
    int err = AR_ERROR_OK;
    LMapDeviceCacheReadLock wl = map_device_cache.getReadLockObject();
    MapDeviceCacheIterator dev_iter = map_device_cache().find(device_index);
    if(dev_iter == map_device_cache().end() ||
       force_update_cache) {
        wl->unlock();
        CDWUniquePtr ds_req_msg(new CDataWrapper());
        CDWShrdPtr answer;
        ds_req_msg->addStringValue("mtype", "dev");
        ds_req_msg->addInt32Value("index", device_index);
        if((err = sendRawRequest(MessageTypeMetadataGetDeviceDataset,
                                 ChaosMoveOperator(ds_req_msg),
                                 answer,
                                 timeout))) {
            return err;
        }
        if(!answer) {
            return  AR_ERROR_INVALID_MESSAGE_STRUCTURE;
        }
        
        wl->lock();
        if(dev_iter == map_device_cache().end()) {
            wl->unlock();
            //we can read the device structure
            ChaosSharedPtr<DeviceCache> device_info = ChaosMakeSharedPtr<DeviceCache>();
            if((err = buildDeviceInfo(*answer,
                                      device_info->device) == 0)) {
                LMapDeviceCacheWriteLock wl = map_device_cache.getWriteLockObject();
                
                DBG << "Create new cache for:" << device_info->device.name;
                //cache is empty
                map_device_cache().insert(MapDeviceCachePair(device_index,
                                                             device_info));
            } else {
                DBG << "Update cache for:" << dev_iter->second->device.name;
                dev_iter->second->device.clear();
                err = buildDeviceInfo(*answer,
                                      dev_iter->second->device);
            }
            
        }
    } else {
        device = dev_iter->second->device;
    }
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::readVariable(const uint32_t device_index,
                                         const VariableType var_type,
                                         const uint32_t var_index,
                                         chaos::common::data::CDataVariant& value) {
    //acquire read lock
    LMapDeviceCacheReadLock rl = map_device_cache.getReadLockObject();
    MapDeviceCacheIterator d_iter = map_device_cache().find(device_index);
    if(d_iter == map_device_cache().end()) return AR_ERROR_NO_CACHED_DEVICE_AT_INDEX;
    //we have the device
    MapVarTypeCacheIterator v_t_cache_iter = d_iter->second->map_type_cache.find(var_type);
    if(v_t_cache_iter == d_iter->second->map_type_cache.end()) return AR_ERROR_NO_CACHED_VARIABLE_TYPE;
    
    MapVarCacheValuesIterator v_value_cache_iter = v_t_cache_iter->second.find(var_index);
    if(v_value_cache_iter == v_t_cache_iter->second.end()) return AR_ERROR_NO_CACHED_VARIABLE_TYPE;
    value = v_value_cache_iter->second;
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::updateVariable(const uint32_t device_index,
                                           const VariableType var_type,
                                           const uint32_t var_index,
                                           uint32_t timeout) {
    int err = AR_ERROR_OK;
    CDWShrdPtr answer;
    MessageType answer_type;
    CDWUniquePtr ds_req_msg(new CDataWrapper());
    ds_req_msg->addInt32Value("d_index", (int32_t)device_index);
    ds_req_msg->addInt32Value("v_type", (int32_t)var_type);
    ds_req_msg->addInt32Value("v_index", (int32_t)var_index);
    if((err = sendRawRequest(MessageTypeVariableRead,
                             ChaosMoveOperator(ds_req_msg),
                             answer,
                             timeout))) {
        return err;
    } else if(!answer) {
        return  AR_ERROR_INVALID_MESSAGE_STRUCTURE;
    } else if((err = getMessageType(*answer,
                                    answer_type)) == 0 &&
              answer_type == MessageTypeVariableRead) {
        //we have te new variable value
        err = updateVariableCachedValue(*answer);
    } else {
        err = AR_ERROR_INVALID_MESSAGE_TYPE;
    }
    return err;
}

int AbstractRemoteIODriver::writeVariable(const uint32_t device_index,
                                          VariableType var_type,
                                          const uint32_t var_index,
                                          const chaos::common::data::CDataVariant& value,
                                          bool wait_confirmation,
                                          uint32_t timeout) {
    int err = AR_ERROR_OK;
    CDWShrdPtr answer;
    MessageType answer_type;
    CDWUniquePtr ds_req_msg(new CDataWrapper());
    ds_req_msg->addInt32Value("d_index", (int32_t)device_index);
    ds_req_msg->addInt32Value("v_type", (int32_t)var_type);
    ds_req_msg->addInt32Value("v_index", (int32_t)var_index);
    ds_req_msg->addVariantValue("v_value", value);
    if(wait_confirmation) {
        if((err = sendRawRequest(MessageTypeVariableWrite,
                                 ChaosMoveOperator(ds_req_msg),
                                 answer,
                                 timeout))) {
            return err;
        } else if(!answer) {
            return  AR_ERROR_INVALID_MESSAGE_STRUCTURE;
        } else if((err = getMessageType(*answer,
                                        answer_type)) == 0 &&
                  answer_type == MessageTypeError) {
            //we have te new variable value
            int remote_error;
            err = getErrorFromMessage(*answer, remote_error);
            if(err == 0) {
                err = remote_error;
            }
        } else {
            err = AR_ERROR_INVALID_MESSAGE_TYPE;
        }
    } else {
        err = sendRawMessage(MessageTypeVariableWrite,
                             ChaosMoveOperator(ds_req_msg));
    }
    
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::sendRawRequest(MessageType message_type,
                                           CDWUniquePtr message_data,
                                           CDWShrdPtr& message_response,
                                           uint32_t timeout) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    switch(conn_phase) {
        case RDConnectionPhaseDisconnected:
            return AR_ERROR_NO_CONNECTION;
            break;
        case RDConnectionPhaseConnected:
            CHAOS_ASSERT(current_connection_identifier().size());
            return AR_ERROR_NOT_AUTORIZED;
            break;
        case RDConnectionPhaseAutorized:
            //we can proceeed
            break;
    }
    AbstractRemoteIODriver::DriverResultFuture request_future;
    DriverResultInfo::DriverResultInfoShrdPtr promise_info(new DriverResultInfo());
    promise_info->request_ts = TimingUtil::getTimeStampInMicroseconds();
    promise_info->request_index = message_counter++;
    CDWUniquePtr ext_message(new CDataWrapper());
    composeRequest(ext_message,
                   promise_info->request_index,
                   message_type,
                   message_data);
    
    //store promises in result map
    LSetPromiseWriteLock lmr_wl = set_p.getWriteLockObject();
    set_p().insert(promise_info);
    
    //send message to driver
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(ext_message));
    //set the
    request_future = promise_info->promise.get_future();
    ChaosFutureStatus f_status = request_future.wait_for(ChaosCronoMilliseconds(timeout));
    if(f_status == ChaosFutureStatus::ready) {
        message_response = request_future.get();
        return AR_ERROR_OK;
    } else {
        return AR_ERROR_TIMEOUT;
    }
}

int AbstractRemoteIODriver::sendRawMessage(MessageType message_type,
                                           CDWUniquePtr message_data) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    switch(conn_phase) {
        case RDConnectionPhaseDisconnected:
            return AR_ERROR_NO_CONNECTION;
            break;
        case RDConnectionPhaseConnected:
            CHAOS_ASSERT(current_connection_identifier().size());
            return AR_ERROR_NOT_AUTORIZED;
            break;
        case RDConnectionPhaseAutorized:
            //we can proceeed
            break;
    }
    
    CDWUniquePtr ext_message(new CDataWrapper());
    composeMessage(ext_message,
                   message_type,
                   message_data);
    
    //send message to driver
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(ext_message));
    //we have connection
    return AR_ERROR_OK;
}

#pragma mark Private Method
int AbstractRemoteIODriver::buildDeviceInfo(CDataWrapper& received_data,
                                            AbstractRemoteIODriver::Device& device_info) {
    int err = AR_ERROR_OK;
    //we have data
    if(received_data.hasKey("device_desc") == false ||
       received_data.isCDataWrapperValue("device_desc") == false) {
        return AR_ERROR_INVALID_MESSAGE_STRUCTURE;
    }
    try {
        CHECK_KEY_THROW_AND_LOG((&received_data), "device_desc", ERR, -1, "device_desc is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(received_data.isCDataWrapperValue("device_desc"), ERR, -2, "device_desc key need to be an object");
        
        ChaosUniquePtr<CDataWrapper> device_structure(received_data.getCSDataValue("device_desc"));
        CHECK_KEY_THROW_AND_LOG(device_structure, "name", ERR, -3, "name is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(device_structure->isCDataWrapperValue("name"), ERR, -4, "name key need to be an object");
        device_info.name = device_structure->getStringValue("name");
        device_info.firmware_v = CDW_GET_SRT_WITH_DEFAULT((&received_data), "firmware_veversion", "");
        
        CHECK_KEY_THROW_AND_LOG(device_structure, "variables", ERR, -5, "device need to be at least a variable");
        CHECK_ASSERTION_THROW_AND_LOG(device_structure->isVectorValue("variables"), ERR, -2, "variables key need to be a vector");
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> variable_array(device_structure->getVectorValue("variables"));
        for(int idx = 0;
            idx < variable_array->size();
            idx++) {
            CHECK_ASSERTION_THROW_AND_LOG(variable_array->isCDataWrapperElementAtIndex(idx), ERR, -6, "variables element array need to be an object");
            DEBUG_CODE(DBG << CHAOS_FORMAT("Add new variable to device %1%", %device_info.name);)
            
            DeviceVariable var;
            ChaosUniquePtr<CDataWrapper> variable_info(variable_array->getCDataWrapperElementAtIndex(idx));
            
            CHECK_KEY_THROW_AND_LOG(variable_info, "name", ERR, -7, "variable name is mandatory");
            CHECK_ASSERTION_THROW_AND_LOG(variable_info->isStringValue("name"), ERR, -8, "variable name key need to be a string");
            var.name = variable_info->getStringValue("name");
            
            CHECK_KEY_THROW_AND_LOG(variable_info, "description", ERR, -9, "variable description is mandatory");
            CHECK_ASSERTION_THROW_AND_LOG(variable_info->isStringValue("description"), ERR, -10, "variable description key need to be a string");
            var.name = variable_info->getStringValue("description");
            
            CHECK_KEY_THROW_AND_LOG(variable_info, "type", ERR, -11, "variable type is mandatory");
            CHECK_ASSERTION_THROW_AND_LOG(variable_info->isInt32Value("type"), ERR, -12, "variable type key need to be an int32");
            var.name = static_cast<VariableType>(variable_info->getInt32Value("type"));
            
            CHECK_KEY_THROW_AND_LOG(variable_info, "operativity", ERR, -12, "variable operativity is mandatory");
            CHECK_ASSERTION_THROW_AND_LOG(variable_info->isInt32Value("operativity"), ERR, -14, "variable operativity key need to be an int32");
            var.admit_operation = variable_info->getInt32Value("operativity");
        }
    } catch (CException& ex) {
        err = ex.errorCode;
    }
    
    return err;
}

int AbstractRemoteIODriver::updateVariableCachedValue(CDataWrapper& received_data) {
    int err = AR_ERROR_OK;
    try {
        CHECK_KEY_THROW_AND_LOG((&received_data), "d_index", ERR, -1, "d_index is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(received_data.isInt32Value("d_index"), ERR, -2, "d_index key need to be an int32");
        CHECK_KEY_THROW_AND_LOG((&received_data), "v_type", ERR, -3, "v_type is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(received_data.isInt32Value("v_type"), ERR, -4, "v_type key need to be an int32");
        CHECK_KEY_THROW_AND_LOG((&received_data), "v_index", ERR, -5, "v_index is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(received_data.isInt32Value("v_index"), ERR, -6, "v_index key need to be an int32");
        CHECK_KEY_THROW_AND_LOG((&received_data), "v_value", ERR, -5, "v_value is mandatory");
        
        const uint32_t d_index = received_data.getInt32Value("d_index");
        const VariableType v_type = static_cast<VariableType>(received_data.getInt32Value("v_type"));
        const uint32_t v_index = received_data.getInt32Value("v_index");
        
        LMapDeviceCacheReadLock rl = map_device_cache.getReadLockObject();
        MapDeviceCacheIterator d_iter = map_device_cache().find(d_index);
        if(d_iter == map_device_cache().end()) return AR_ERROR_NO_CACHED_DEVICE_AT_INDEX;
        //we have the device
        MapVarTypeCacheIterator v_t_cache_iter = d_iter->second->map_type_cache.find(v_type);
        if(v_t_cache_iter == d_iter->second->map_type_cache.end()) return AR_ERROR_NO_CACHED_VARIABLE_TYPE;
        
        MapVarCacheValuesIterator v_value_cache_iter = v_t_cache_iter->second.find(v_index);
        if(v_value_cache_iter == v_t_cache_iter->second.end()) return AR_ERROR_NO_CACHED_VARIABLE_TYPE;
        v_value_cache_iter->second = received_data.getVariantValue("v_value");
    } catch (CException& ex) {
        err = ex.errorCode;
    }
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::getMessageType(CDataWrapper& received_data,
                                           MessageType& type) {
    int err = AR_ERROR_OK;
    try {
        CHECK_KEY_THROW_AND_LOG((&received_data), "type", ERR, -1, "type is mandatory");
        CHECK_ASSERTION_THROW_AND_LOG(received_data.isInt32Value("type"), ERR, -2, "type key need to be an int32");
        type = static_cast<MessageType>(received_data.getInt32Value("type"));
    } catch (CException& ex) {
        err = ex.errorCode;
    }
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::getErrorFromMessage(chaos::common::data::CDataWrapper& received_data,
                                                int& error_code) {
    int err = AR_ERROR_OK;
    if(received_data.hasKey("error_code") == false) {return AR_ERROR_INVALID_MESSAGE_STRUCTURE;}
    if(received_data.isStringValue("error_code") == false) {return AR_ERROR_INVALID_MESSAGE_STRUCTURE;}
    error_code = received_data.getInt32Value("error_code");
    return err;
}
