/*
 *	NodeController.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/03/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/node_monitor/NodeController.h>

#include <boost/format.hpp>

#define RETRY_TIME_FOR_OFFLINE 6

#define NC_LINFO INFO_LOG(NodeController)
#define NC_LDBG DBG_LOG(NodeController)
#define NC_LERR ERR_LOG(NodeController)

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::node_monitor;
using namespace chaos::metadata_service_client::monitor_system;

NodeController::NodeController(const std::string& _node_uid):
node_uid(_node_uid),
node_health_uid(boost::str(boost::format("%1%%2%")%node_uid%chaos::DataPackPrefixID::HEALTH_DATASET_POSTFIX)) {
    //add common node dataset
    monitor_key_list.push_back(node_health_uid);
    
    _resetHealth();
    
}

NodeController::~NodeController() {}

const std::string& NodeController::getNodeUID() {
    return node_uid;
}

const MonitorKeyList& NodeController::getMonitorKeyList() const {
    return monitor_key_list;
}

const MonitoHandlerList& NodeController::getMonitorHandlerList() const {
    return list_handler;
}

const HealthInformation& NodeController::getHealthInformation() const {
    return health_info;
}

const unsigned int NodeController::getHandlerListSise() {
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    return (unsigned int)list_handler.size();
}

void NodeController::updateData() {
    //check for mandatory key
    if(!last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) ||
       !last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS)) return;
    
    map_ds_health.clear();
    
    int value_type = 0;
    std::vector<std::string> key_names;
    last_ds_healt->getAllKey(key_names);
    for(std::vector<std::string>::iterator it = key_names.begin(),
        end = key_names.end();
        it != end;
        it++) {
        switch((value_type = last_ds_healt->getValueType(*it))) {
            case CDataWrapperTypeBool:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getBoolValue(*it));
                break;
            case CDataWrapperTypeInt32:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getInt32Value(*it));
                break;
            case CDataWrapperTypeInt64:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getInt64Value(*it));
                break;
            case CDataWrapperTypeDouble:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getDoubleValue(*it));
                break;
            case CDataWrapperTypeString:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getStringValue(*it));
                break;
            case CDataWrapperTypeBinary:
                map_ds_health[*it] = CDataVariant(last_ds_healt->getBinaryValueAsCDataBuffer(*it).release());
                break;
                
            default:
                break;
        }
    }
    
    uint64_t received_ts = last_ds_healt->getUInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP);
    if(last_recevied_ts == 0) {
        last_recevied_ts = received_ts;
        //unknown
        _setOnlineState(OnlineStateUnknown);
    } else {
        if((received_ts - last_recevied_ts) > 0) {
            //online
            was_online = true;
            zero_diff_count_on_ts = 0;
            _setOnlineState(OnlineStateON);
        } else {
            if(((++zero_diff_count_on_ts > RETRY_TIME_FOR_OFFLINE) == true) ||
               (last_recevied_ts == 0)||
               (was_online == false)) {
                //offline
                _setOnlineState(OnlineStateOFF);
            } else {
                if(last_recevied_ts == 0) {
                    //unknown
                    _setOnlineState(OnlineStateUnknown);
                }
            }
        }
        //keep track of current timestamp
        last_recevied_ts = received_ts;
        
        //update internal state
        _setNodeInternalState(last_ds_healt->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS));
        
        if(last_received_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR) == 0 ||
           last_received_status.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR) == 0) {
            if(last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE) &&
               last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE) &&
               last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE)) {
                //we need to show error
                ErrorInformation new_err_inf;
                new_err_inf.error_code = last_ds_healt->getInt32Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE);
                new_err_inf.error_message = last_ds_healt->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE);
                new_err_inf.error_domain = last_ds_healt->getStringValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN);
                _setError(new_err_inf);
            }else{
                _setError(ErrorInformation());
            }
        } else {
            _setError(ErrorInformation());
        }
        
        if(last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME) &&
           last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME) &&
           last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME) &&
           last_ds_healt->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP)) {
            ProcessResource proc_res;
            proc_res.uptime = last_ds_healt->getUInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME);
            proc_res.usr_res = last_ds_healt->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME);
            proc_res.sys_res = last_ds_healt->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME);
            proc_res.swp_res = last_ds_healt->getInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP);
            _setProcessResource(proc_res);
        } else {
            _setProcessResource(ProcessResource());
        }
    }
    
    _fireHealthDatasetChanged();
}

void NodeController::quantumSlotHasData(const std::string& key,
                                        const monitor_system::KeyValue& value) {
    //check for monitored key
    if(key.compare(node_health_uid) != 0) return;
    //update internal structure
    last_ds_healt = value;
    //reset all states
    updateData();
    
}

void NodeController::quantumSlotHasNoData(const std::string& key) {
    if(key.compare(node_health_uid) == 0) {
        _setOnlineState(OnlineStateNotFound);
    }
}

void NodeController::_resetHealth() {
    map_ds_health.clear();
    health_info.online_state = OnlineStateUnknown;
    health_info.internal_state = "Undefined";
    health_info.process_resource.uptime = 0;
    health_info.process_resource.usr_res = 0.0;
    health_info.process_resource.sys_res = 0.0;
    health_info.process_resource.swp_res = 0.0;
    
    health_info.error_information.error_code = 0;
    health_info.error_information.error_message.clear();
    health_info.error_information.error_domain.clear();
    
    zero_diff_count_on_ts = 0;
    last_recevied_ts = 0;
    was_online = false;
}

void NodeController::_fireHealthDatasetChanged(){
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for(MonitoHandlerListIterator it = list_handler.begin(),
        it_end = list_handler.end();
        it != it_end;
        it++) {
        //notify listers that online status has been changed
        CHAOS_NOT_THROW((*it)->nodeChangedHealthDataset(node_uid,
                                                        map_ds_health););
    }
}

void NodeController::_setOnlineState(const OnlineState new_online_state) {
    bool changed = health_info.online_state != new_online_state;
    
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for(MonitoHandlerListIterator it = list_handler.begin(),
        it_end = list_handler.end();
        it != it_end;
        it++) {
        if((*it)->signalOnChange() == true && changed == false) {continue;}
        //notify listers that online status has been changed
        CHAOS_NOT_THROW((*it)->nodeChangedOnlineState(node_uid,
                                                      health_info.online_state,
                                                      new_online_state););
    }
    if(changed) {
        health_info.online_state = new_online_state;
        //add online state into map
        map_ds_health[NodeMonitorHandler::MAP_KEY_ONLINE_STATE] = CDataVariant((int32_t)new_online_state);
    }
}


void NodeController::_setNodeInternalState(const std::string& new_internal_state) {
    bool changed = health_info.internal_state.compare(new_internal_state) != 0;
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for(MonitoHandlerListIterator it = list_handler.begin(),
        it_end = list_handler.end();
        it != it_end;
        it++) {
        if((*it)->signalOnChange() == true && changed == false) {continue;}
        //notify listers that online status has been changed
        CHAOS_NOT_THROW((*it)->nodeChangedInternalState(node_uid,
                                                        health_info.internal_state,
                                                        new_internal_state););
    }
    if(changed){health_info.internal_state = new_internal_state;}
}

void NodeController::_setError(const ErrorInformation& new_error_information) {
    bool changed = (health_info.error_information.error_code != new_error_information.error_code) ||
    (health_info.error_information.error_message.compare(new_error_information.error_message) != 0) ||
    (health_info.error_information.error_domain.compare(new_error_information.error_domain) != 0);
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for(MonitoHandlerListIterator it = list_handler.begin(),
        it_end = list_handler.end();
        it != it_end;
        it++) {
        if((*it)->signalOnChange() == true && changed == false) {continue;}
        //notify listers that online status has been changed
        CHAOS_NOT_THROW((*it)->nodeChangedErrorInformation(node_uid,
                                                           health_info.error_information,
                                                           new_error_information););
    }
    if(changed){health_info.error_information = new_error_information;}
}

void NodeController::_setProcessResource(const ProcessResource& new_process_resource) {
    bool changed = (health_info.process_resource.uptime != new_process_resource.uptime)||
    (health_info.process_resource.usr_res != new_process_resource.usr_res) ||
    (health_info.process_resource.sys_res != new_process_resource.sys_res) ||
    (health_info.process_resource.swp_res != new_process_resource.swp_res);
    
    bool restarted = changed && health_info.process_resource.uptime > new_process_resource.uptime;
    
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for(MonitoHandlerListIterator it = list_handler.begin(),
        it_end = list_handler.end();
        it != it_end;
        it++) {
        if(restarted) {
            //notify listers that online status has been changed
            CHAOS_NOT_THROW((*it)->nodeHasBeenRestarted(node_uid););
        }
        if((*it)->signalOnChange() == true && changed == false) {continue;}
        //notify listers that online status has been changed
        CHAOS_NOT_THROW((*it)->nodeChangedProcessResource(node_uid,
                                                          health_info.process_resource,
                                                          new_process_resource););
    }
    if(changed){health_info.process_resource = new_process_resource;}
}

bool NodeController::addHandler(NodeMonitorHandler *handler_to_add) {
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    if(list_handler.find(handler_to_add) != list_handler.end()) return false;
    list_handler.insert(handler_to_add);
    //fire current state to the handler
    CHAOS_NOT_THROW(handler_to_add->nodeChangedOnlineState(node_uid,
                                                           health_info.online_state,
                                                           health_info.online_state););
    CHAOS_NOT_THROW(handler_to_add->nodeChangedInternalState(node_uid,
                                                             health_info.internal_state,
                                                             health_info.internal_state););
    CHAOS_NOT_THROW(handler_to_add->nodeChangedErrorInformation(node_uid,
                                                                health_info.error_information,
                                                                health_info.error_information););
    CHAOS_NOT_THROW(handler_to_add->nodeChangedProcessResource(node_uid,
                                                               health_info.process_resource,
                                                               health_info.process_resource););
    CHAOS_NOT_THROW(handler_to_add->nodeChangedHealthDataset(node_uid,
                                                             map_ds_health););
    return true;
}

bool NodeController::removeHandler(NodeMonitorHandler *handler_to_remove) {
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    if(list_handler.find(handler_to_remove) == list_handler.end()) return false;
    list_handler.erase(handler_to_remove);
    return true;
}
