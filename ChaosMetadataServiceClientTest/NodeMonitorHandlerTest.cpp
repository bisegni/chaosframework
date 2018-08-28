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
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            return "Alert";
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            return "Warning";
            break;
    }
    return "";
}

NodeMonitorHandlerTest::NodeMonitorHandlerTest(const std::string& _node_uid,
                                               chaos::metadata_service_client::node_monitor::ControllerType _controller_type):
node_uid(_node_uid),
controller_type(_controller_type),
last_ts(-1){
    setSignalOnChange(false);
    ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid,
                                                                       controller_type,
                                                                       this);
}

NodeMonitorHandlerTest::~NodeMonitorHandlerTest() {
    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid,
                                                                          controller_type,
                                                                          this);
}

void NodeMonitorHandlerTest::nodeChangedOnlineState(const std::string& node_uid,
                                                    OnlineState old_status,
                                                    OnlineState new_status) {
    LAPP_ << node_uid << " - nodeChangedOnlineState: " << getStatusDesc(new_status)<<"["<<getStatusDesc(old_status)<<"]";
}


void NodeMonitorHandlerTest::nodeChangedInternalState(const std::string& node_uid,
                                                      const std::string& old_status,
                                                      const std::string& new_status) {
    LAPP_ << node_uid << " - nodeChangedInternalState: " << new_status<<"["<<old_status<<"]";
}

void NodeMonitorHandlerTest::nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res) {
    LAPP_ << boost::str(boost::format("%5% nodeChangedProcessResource: usr:%1% sys:%2% swp:%3% upt:%4%")%new_proc_res.usr_res%new_proc_res.sys_res%new_proc_res.swp_res%new_proc_res.uptime%node_uid);
    
}

void NodeMonitorHandlerTest::nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_status,
                                                         const ErrorInformation& new_status) {
    LAPP_<< node_uid << "nodeChangedErrorInformation: ";
    
}

void NodeMonitorHandlerTest::nodeHasBeenRestarted(const std::string& node_uid) {
    LAPP_<< node_uid << "nodeHasBeenRestarted: " << node_uid;
}

void NodeMonitorHandlerTest::updatedDS(const std::string& control_unit_uid,
                                       int dataset_type,
                                       MapDatasetKeyValues& dataset_key_values) {
    LAPP_ << boost::str(boost::format("%2% updatedDS: dataset type %1%")%getDatasetDesc(dataset_type)%node_uid);
    switch (dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT: {
            if(dataset_key_values.count(chaos::DataPackCommonKey::DPCK_TIMESTAMP)) {
                CDataVariant variant = dataset_key_values[chaos::DataPackCommonKey::DPCK_TIMESTAMP];
                LAPP_ << node_uid << " DPCK_DATASET_TYPE_OUTPUT " <<variant.asInt64();
                if(last_ts == -1) {
                    last_ts = variant.asInt64();
                } else {
                    assert(last_ts == variant.asInt64());
                }
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
