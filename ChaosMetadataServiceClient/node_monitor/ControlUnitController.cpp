/*
 *	ControlUnitMonitor.cpp
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
#include <chaos/common/chaos_constants.h>

#include <ChaosMetadataServiceClient/node_monitor/ControlUnitController.h>
#include <ChaosMetadataServiceClient/node_monitor/ControlUnitMonitorHandler.h>

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::node_monitor;

ControlUnitController::ControlUnitController(const std::string &_node_uid):
NodeController(_node_uid),
cu_output_ds_key(boost::str(boost::format("%1%%2%") % getNodeUID() % chaos::DataPackPrefixID::OUTPUT_DATASET_POSTFIX)),
cu_input_ds_key(boost::str(boost::format("%1%%2%") % getNodeUID() % chaos::DataPackPrefixID::INPUT_DATASET_POSTFIX)),
cu_system_ds_key(boost::str(boost::format("%1%%2%") % getNodeUID() % chaos::DataPackPrefixID::SYSTEM_DATASET_POSTFIX)),
cu_alarm_ds_key(boost::str(boost::format("%1%%2%") % getNodeUID() % chaos::DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX)),
cu_warning_ds_key(boost::str(boost::format("%1%%2%") % getNodeUID() % chaos::DataPackPrefixID::CU_ALARM_DATASET_POSTFIX)){
    monitor_key_list.push_back(cu_output_ds_key);
    monitor_key_list.push_back(cu_input_ds_key);
    monitor_key_list.push_back(cu_system_ds_key);
    monitor_key_list.push_back(cu_alarm_ds_key);
    monitor_key_list.push_back(cu_warning_ds_key);
}

ControlUnitController::~ControlUnitController() { }

void ControlUnitController::quantumSlotHasData(const std::string &key,
                                               const monitor_system::KeyValue &value) {
    bool changed = false;
    if (key.compare(cu_output_ds_key) == 0) {
        if((changed = CHECK_DS_CHANGED(last_ds_output, value))) {
            //update output datset key
            _updateDatsetKeyMapValue((last_ds_output = value),
                                     map_ds_out);
        }
        //call handler
        _fireUpdateDSOnHandler(DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                               map_ds_out,
                               changed);
    } else if (key.compare(cu_input_ds_key) == 0) {
        if((changed = CHECK_DS_CHANGED(last_ds_input, value))) {
            //update input datset key
            _updateDatsetKeyMapValue((last_ds_input = value),
                                     map_ds_in);
        }
        //call handler
        _fireUpdateDSOnHandler(DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                               map_ds_in,
                               changed);
    } else if (key.compare(cu_system_ds_key) == 0) {
        if((changed = CHECK_DS_CHANGED(last_ds_system, value))) {
            //update input datset key
            _updateDatsetKeyMapValue((last_ds_system = value),
                                     map_ds_sys);
        }
        //call handler
        _fireUpdateDSOnHandler(DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
                               map_ds_sys,
                               changed);
    } else if (key.compare(cu_alarm_ds_key) == 0) {
        if((changed = CHECK_DS_CHANGED(last_ds_alarm, value))){
            //update input datset key
            _updateDatsetKeyMapValue((last_ds_alarm = value),
                                     map_ds_alarm);
        }
        //call handler
        _fireUpdateDSOnHandler(DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM,
                               map_ds_alarm,
                               changed);
    } else if (key.compare(cu_warning_ds_key) == 0) {
        if((changed = CHECK_DS_CHANGED(last_ds_warning, value))){
            //update input datset key
            _updateDatsetKeyMapValue((last_ds_warning = value),
                                     map_ds_warning);
        }
        //call handler
        _fireUpdateDSOnHandler(DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM,
                               map_ds_warning,
                               changed);
    } else {
        NodeController::quantumSlotHasData(key,
                                           value);
    }
}

void ControlUnitController::quantumSlotHasNoData(const std::string &key) {
    if (key.compare(cu_output_ds_key) == 0 ||
        key.compare(cu_input_ds_key) == 0 ||
        key.compare(cu_system_ds_key) == 0 ||
        key.compare(cu_alarm_ds_key) == 0 ||
        key.compare(cu_warning_ds_key) == 0) {
    } else {
        NodeController::quantumSlotHasNoData(key);
    }
}


void ControlUnitController::_updateDatsetKeyMapValue(chaos::metadata_service_client::monitor_system::KeyValue dataset,
                                                     MapDatasetKeyValues &map) {
    int value_type = 0;
    std::vector<std::string> key_names;
    dataset->getAllKey(key_names);
    for (std::vector<std::string>::iterator it = key_names.begin(),
         end = key_names.end();
         it != end;
         it++) {
        switch ((value_type = dataset->getValueType(*it))) {
            case CDataWrapperTypeBool:
                map[*it] = CDataVariant(dataset->getBoolValue(*it));
                break;
            case CDataWrapperTypeInt32:
                map[*it] = CDataVariant(dataset->getInt32Value(*it));
                break;
            case CDataWrapperTypeInt64:
                map[*it] = CDataVariant(dataset->getInt64Value(*it));
                break;
            case CDataWrapperTypeDouble:
                map[*it] = CDataVariant(dataset->getDoubleValue(*it));
                break;
            case CDataWrapperTypeString:
                map[*it] = CDataVariant(dataset->getStringValue(*it));
                break;
            case CDataWrapperTypeBinary:
                map[*it] = CDataVariant(dataset->getBinaryValueAsCDataBuffer(*it).release());
                break;
                
            default:
                break;
        }
    }
}

void ControlUnitController::_fireUpdateDSOnHandler(int dataset_type,
                                                   MapDatasetKeyValues &map,
                                                   bool changed) {
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for (MonitoHandlerListIterator it = list_handler.begin(),
         it_end = list_handler.end();
         it != it_end;
         it++) {
        ControlUnitMonitorHandler *hndlr = dynamic_cast<ControlUnitMonitorHandler *>(*it);
        if (hndlr == NULL) {
            continue;
        }
        if(hndlr->signalOnChange() == true && changed == false) {continue;}
        
        //notify listers that online status has been changed
        hndlr->updatedDS(getNodeUID(),
                         dataset_type,
                         map);
    }
}

void ControlUnitController::_fireNoDSDataFoundOnHandler(int dataset_type) {
    boost::unique_lock<boost::mutex> wl(list_handler_mutex);
    for (MonitoHandlerListIterator it = list_handler.begin(),
         it_end = list_handler.end();
         it != it_end;
         it++) {
        ControlUnitMonitorHandler *hndlr = dynamic_cast<ControlUnitMonitorHandler *>(*it);
        if (hndlr == NULL) {
            continue;
        }
        //notify listers that online status has been changed
        hndlr->noDSDataFound(getNodeUID(),
                             dataset_type);
    }
}

bool ControlUnitController::addHandler(NodeMonitorHandler *handler_to_add) {
    ControlUnitMonitorHandler *cu_handler = dynamic_cast<ControlUnitMonitorHandler *>(handler_to_add);
    if (cu_handler == NULL) {
        return false;
    }
    bool result = NodeController::addHandler(handler_to_add);
    if (result) {
        cu_handler->updatedDS(getNodeUID(),
                              DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                              map_ds_out);
        cu_handler->updatedDS(getNodeUID(),
                              DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                              map_ds_in);
        cu_handler->updatedDS(getNodeUID(),
                              DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
                              map_ds_sys);
        cu_handler->updatedDS(getNodeUID(),
                              DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM,
                              map_ds_alarm);
        cu_handler->updatedDS(getNodeUID(),
                              DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM,
                              map_ds_warning);
        
    }
    return result;
}
