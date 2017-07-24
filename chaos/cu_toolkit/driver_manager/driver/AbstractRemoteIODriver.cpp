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
            // spontaneus message from driver, that can forward error message
            // or read answer for variable change
            
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
    CDWUniquePtr ds_req_msg(new CDataWrapper());
    CDWShrdPtr answer;
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
                                                 uint32_t timeout) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::readVariable(const uint32_t device_index,
                                         VariableType var_type,
                                         const uint32_t var_index,
                                         const chaos::common::data::CDataVariant& value,
                                         uint32_t timeout) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::updateVariable(const uint32_t device_index,
                                           VariableType var_type,
                                           const uint32_t var_index,
                                           const chaos::common::data::CDataVariant& value,
                                           uint32_t timeout) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::writeVariable(const uint32_t device_index,
                                          VariableType var_type,
                                          const uint32_t var_index,
                                          const chaos::common::data::CDataVariant& value,
                                          uint32_t timeout) {
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
