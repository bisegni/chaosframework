//
//  NodeMonitorHandlerTest.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/16.
//  Copyright © 2016 INFN. All rights reserved.
//

#include "NodeMonitorHandlerTest.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client;

std::string getStatusDesc(chaos::metadata_service_client::node_monitor::OnlineStatus status){
    switch(status){
        case chaos::metadata_service_client::node_monitor::OnlineStatusUnknown:
            return "Unknown";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStatusON:
            return "Status ON";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStatusOFF:
            return "Status OFF";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStatusNotFound:
            return "No status data found";
            break;
    }
}

std::string getDatasetDesc(int dataset_type){
    switch(dataset_type){
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            return "Output";
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            return "Input";
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            return "System";
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            return "Custom";
            break;
    }
    return "";
}

NodeMonitorHandlerTest::NodeMonitorHandlerTest(const std::string& _node_uid):
node_uid(_node_uid){
    ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid,
                                                                       chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                       this);
}

NodeMonitorHandlerTest::~NodeMonitorHandlerTest() {
    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid,
                                                                          chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                          this);
}

using namespace chaos::metadata_service_client::node_monitor;

void NodeMonitorHandlerTest::nodeChangedOnlineStatus(const std::string& node_uid,
                                                     OnlineStatus old_status,
                                                     OnlineStatus new_status) {
    LAPP_ << "nodeChangedOnlineStatus: " << getStatusDesc(new_status)<<"["<<getStatusDesc(old_status)<<"]";
}

void NodeMonitorHandlerTest::nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res) {
    LAPP_ << boost::str(boost::format("nodeChangedProcessResource: usr:%1% sys:%2% swp:%3% upt:%4%")%new_proc_res.usr_res%new_proc_res.sys_res%new_proc_res.swp_res%new_proc_res.uptime);
    
}

void NodeMonitorHandlerTest::nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_status,
                                                         const ErrorInformation& new_status) {
    LAPP_<< "nodeChangedErrorInformation: ";
    
}

void NodeMonitorHandlerTest::updatedDS(const std::string& control_unit_uid,
                                       int dataset_type,
                                       MapDatasetKeyValues& dataset_key_values) {
    LAPP_ << boost::str(boost::format("updatedDS: dataset type %1%")%getDatasetDesc(dataset_type));
    switch (dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT: {
            CDataVariant variant = dataset_key_values[chaos::DataPackCommonKey::DPCK_TIMESTAMP];
            LAPP_ << variant.asInt64();
            break;
        }
            
        default:
            break;
    }
}

void NodeMonitorHandlerTest::noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type) {
    
}

void NodeMonitorHandlerTest::handlerHasBeenRegistered(const std::string& node_uid,
                                                      const HealthInformation& current_health_status) {
    
}

void NodeMonitorHandlerTest::handlerHasBeenRegistered(const std::string& control_unit_uid,
                                                      int dataset_type,
                                                      MapDatasetKeyValues& dataset_key_values) {
    
}