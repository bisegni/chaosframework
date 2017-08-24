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

#include <chaos_metadata_service_client/node_monitor/NodeMonitorHandler.h>

using namespace chaos::metadata_service_client::node_monitor;

const char * const NodeMonitorHandler::MAP_KEY_ONLINE_STATE = "OnlineState";

NodeMonitorHandler::NodeMonitorHandler():
handler_uuid(chaos::common::utility::UUIDUtil::generateUUIDLite()),
signal_on_change(true){}

NodeMonitorHandler::~NodeMonitorHandler() {}

void NodeMonitorHandler::nodeHasBeenRestarted(const std::string& node_uid) {
    
}

void NodeMonitorHandler::nodeChangedOnlineState(const std::string& node_uid,
                                                OnlineState old_status,
                                                OnlineState new_status) {
    
}

void NodeMonitorHandler::nodeChangedInternalState(const std::string& node_uid,
                                                  const std::string& old_status,
                                                  const std::string& new_status) {
    
}

void NodeMonitorHandler::nodeChangedProcessResource(const std::string& node_uid,
                                                    const ProcessResource& old_proc_res,
                                                    const ProcessResource& new_proc_res) {
    
}

void NodeMonitorHandler::nodeChangedErrorInformation(const std::string& node_uid,
                                                     const ErrorInformation& old_status,
                                                     const ErrorInformation& new_status) {
    
}

void NodeMonitorHandler::nodeChangedHealthDataset(const std::string& node_uid,
                                                  MapDatasetKeyValues& map_health_dataset) {
    
}

void NodeMonitorHandler::setSignalOnChange(bool new_change_on_signal) {
    signal_on_change = new_change_on_signal;
}

bool NodeMonitorHandler::signalOnChange() {
    return signal_on_change;
}
