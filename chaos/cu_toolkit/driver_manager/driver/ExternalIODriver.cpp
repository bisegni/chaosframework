/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/cu_toolkit/driver_manager/driver/ExternalIODriver.h>

using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;

#define INFO    INFO_LOG(ExternalIODriver)
#define DBG		DBG_LOG(ExternalIODriver)
#define ERR		ERR_LOG(ExternalIODriver)

template <typename ExtDriverImpl>
ExternalIODriver<ExtDriverImpl>::ExternalIODriver() {
    
}

template <typename ExtDriverImpl>
ExternalIODriver<ExtDriverImpl>::~ExternalIODriver() {
    
}

#pragma mark Public API
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::getDeviceList(ChaosStringSet& hosted_device,
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
        return  IO_ERROR_INVALID_MESSAGE_STRUCTURE;
    }
    //we have data
    if(answer->hasKey("device_list") == false ||
       answer->isVectorValue("device_list") == false) {
        return IO_ERROR_INVALID_MESSAGE_STRUCTURE;
    }
    
    //we can get the list
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> arr(answer->getVectorValue("device_list"));
    for(int idx = 0;
        idx < arr->size();
        idx++) {
        if(arr->isStringElementAtIndex(idx) == false) {
            ERR <<"Device element in array is not a string";
            hosted_device.clear();
            return IO_ERROR_INVALID_MESSAGE_STRUCTURE;
        }
        hosted_device.insert(arr->getStringElementAtIndex(idx));
    }
    return err;
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::getDeviceDescription(const int32_t& device_index,
                                           Device& device,
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
            return  IO_ERROR_INVALID_MESSAGE_STRUCTURE;
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

template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::readVariable(const uint32_t device_index,
                                   const VariableType var_type,
                                   const uint32_t var_index,
                                   chaos::common::data::CDataVariant& value) {
    //acquire read lock
    LMapDeviceCacheReadLock rl = map_device_cache.getReadLockObject();
    MapDeviceCacheIterator d_iter = map_device_cache().find(device_index);
    if(d_iter == map_device_cache().end()) return IO_ERROR_NO_CACHED_DEVICE_AT_INDEX;
    //we have the device
    MapVarTypeCacheIterator v_t_cache_iter = d_iter->second->map_type_cache.find(var_type);
    if(v_t_cache_iter == d_iter->second->map_type_cache.end()) return IO_ERROR_NO_CACHED_VARIABLE_TYPE;
    
    MapVarCacheValuesIterator v_value_cache_iter = v_t_cache_iter->second.find(var_index);
    if(v_value_cache_iter == v_t_cache_iter->second.end()) return IO_ERROR_NO_CACHED_VARIABLE_TYPE;
    value = v_value_cache_iter->second;
    return AR_ERROR_OK;
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::updateVariable(const uint32_t device_index,
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
        return  IO_ERROR_INVALID_MESSAGE_STRUCTURE;
    } else if((err = getMessageType(*answer,
                                    answer_type)) == 0 &&
              answer_type == MessageTypeVariableRead) {
        //we have te new variable value
        err = updateVariableCachedValue(*answer);
    } else {
        err = IO_ERROR_INVALID_MESSAGE_TYPE;
    }
    return err;
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::writeVariable(const uint32_t device_index,
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
            return  IO_ERROR_INVALID_MESSAGE_STRUCTURE;
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
            err = IO_ERROR_INVALID_MESSAGE_TYPE;
        }
    } else {
        err = sendRawMessage(MessageTypeVariableWrite,
                             ChaosMoveOperator(ds_req_msg));
    }
    
    return AR_ERROR_OK;
}

#pragma mark Private Method
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::buildDeviceInfo(CDataWrapper& received_data,
                                      Device& device_info) {
    int err = AR_ERROR_OK;
    //we have data
    if(received_data.hasKey("device_desc") == false ||
       received_data.isCDataWrapperValue("device_desc") == false) {
        return IO_ERROR_INVALID_MESSAGE_STRUCTURE;
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
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::updateVariableCachedValue(CDataWrapper& received_data) {
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
        if(d_iter == map_device_cache().end()) return IO_ERROR_NO_CACHED_DEVICE_AT_INDEX;
        //we have the device
        MapVarTypeCacheIterator v_t_cache_iter = d_iter->second->map_type_cache.find(v_type);
        if(v_t_cache_iter == d_iter->second->map_type_cache.end()) return IO_ERROR_NO_CACHED_VARIABLE_TYPE;
        
        MapVarCacheValuesIterator v_value_cache_iter = v_t_cache_iter->second.find(v_index);
        if(v_value_cache_iter == v_t_cache_iter->second.end()) return IO_ERROR_NO_CACHED_VARIABLE_TYPE;
        v_value_cache_iter->second = received_data.getVariantValue("v_value");
    } catch (CException& ex) {
        err = ex.errorCode;
    }
    return AR_ERROR_OK;
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::getMessageType(CDataWrapper& received_data,
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
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::getErrorFromMessage(chaos::common::data::CDataWrapper& received_data,
                                          int& error_code) {
    int err = AR_ERROR_OK;
    if(received_data.hasKey("error_code") == false) {return IO_ERROR_INVALID_MESSAGE_STRUCTURE;}
    if(received_data.isStringValue("error_code") == false) {return IO_ERROR_INVALID_MESSAGE_STRUCTURE;}
    error_code = received_data.getInt32Value("error_code");
    return err;
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::sendRawRequest(MessageType message_type,
                                     CDWUniquePtr message_data,
                                     CDWShrdPtr& message_response,
                                     uint32_t timeout) {
    message_data->addInt32Value("type", (int32_t)message_type);
    return ExtDriverImpl::sendRawRequest(ChaosMoveOperator(message_data),
                                                  message_response,
                                                  timeout);
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::sendRawMessage(MessageType message_type,
                                     CDWUniquePtr message_data) {
    message_data->addInt32Value("type", (int32_t)message_type);
    return ExtDriverImpl::sendRawMessage(ChaosMoveOperator(message_data));
}
template <typename ExtDriverImpl>
int ExternalIODriver<ExtDriverImpl>::asyncMessageReceived(chaos::common::data::CDWUniquePtr message) {
    int err = AR_ERROR_OK;
    MessageType received_message_type;
    // spontaneus message from driver, that can forward error message
    // or read answer for variable change
    if((err = getMessageType(*message,
                             received_message_type))) {
        ERR << "Error decoding received message type";
        return IO_ERROR_INVALID_MESSAGE_TYPE;
    }
    
    if(received_message_type == MessageTypeVariableRead) {
        //we have te new variable value
        err = updateVariableCachedValue(*message);
    }
    return err;
}
