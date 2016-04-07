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
using namespace chaos::metadata_service_client::node_monitor;

std::string getStatusDesc(chaos::metadata_service_client::node_monitor::OnlineState status){
    switch(status){
        case chaos::metadata_service_client::node_monitor::OnlineStateUnknown:
            return "Unknown";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStateON:
            return "Status ON";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStateOFF:
            return "Status OFF";
            break;
            
        case chaos::metadata_service_client::node_monitor::OnlineStateNotFound:
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

void NodeMonitorHandlerTest::nodeChangedOnlineState(const std::string& node_uid,
                                                    OnlineState old_status,
                                                    OnlineState new_status) {
    LAPP_ << "nodeChangedOnlineState: " << getStatusDesc(new_status)<<"["<<getStatusDesc(old_status)<<"]";
}


void NodeMonitorHandlerTest::nodeChangedInternalState(const std::string& node_uid,
                                                      const std::string& old_status,
                                                      const std::string& new_status) {
    LAPP_ << "nodeChangedInternalState: " << new_status<<"["<<old_status<<"]";
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

void NodeMonitorHandlerTest::nodeHasBeenRestarted(const std::string& node_uid) {
    LAPP_<< "nodeHasBeenRestarted: " << node_uid;
}

void NodeMonitorHandlerTest::updatedDS(const std::string& control_unit_uid,
                                       int dataset_type,
                                       MapDatasetKeyValues& dataset_key_values) {
    LAPP_ << boost::str(boost::format("updatedDS: dataset type %1%")%getDatasetDesc(dataset_type));
    switch (dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT: {
            if(dataset_key_values.count(chaos::DataPackCommonKey::DPCK_TIMESTAMP)) {
                CDataVariant variant = dataset_key_values[chaos::DataPackCommonKey::DPCK_TIMESTAMP];
                LAPP_ << variant.asInt64();
            }
            break;
        }
            
        default:
            break;
    }
}

void NodeMonitorHandlerTest::noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type) {
    
}