//
//  NodeMonitorHandlerTest.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/16.
//  Copyright © 2016 INFN. All rights reserved.
//

#include "NodeMonitorHandlerTest.h"
#include <chaos/common/utility/Base64Util.h>
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

std::string getStatusDesc(chaos::metadata_service_client::node_monitor::OnlineState status){
    switch(status){
        case chaos::metadata_service_client::node_monitor::OnlineStateON:
            return "Status ON";
            break;
        case chaos::metadata_service_client::node_monitor::OnlineStateOFF:
            return "Status OFF";
            break;
        case chaos::metadata_service_client::node_monitor::OnlineStateNotFound:
            return "No status data found";
            break;
        case chaos::metadata_service_client::node_monitor::OnlineStateUnknown:
        break;
    }
    return "Unknown";
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
                                               chaos::metadata_service_client::node_monitor::ControllerType _controller_type,
                                               const ChaosStringVector& _attribute_to_show):
node_uid(_node_uid),
controller_type(_controller_type),
attribute_to_show(_attribute_to_show){
    setSignalOnChange(true);
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
            std::string output_line;
            if(dataset_key_values.count(chaos::DataPackCommonKey::DPCK_TIMESTAMP)) {
                CDataVariant variant = dataset_key_values[chaos::DataPackCommonKey::DPCK_TIMESTAMP];
                output_line.append(CHAOS_FORMAT("%1%=%2% ", %chaos::DataPackCommonKey::DPCK_TIMESTAMP%variant.asInt64()));
            }
            
            for(ChaosStringVectorConstIterator it = attribute_to_show.begin(),
                end = attribute_to_show.end();
                it != end;
                it++) {
                if(dataset_key_values.count(*it)){
                    CDataVariant attr_variant = dataset_key_values[*it];
                    CDBufferShrdPtr buf = attr_variant.asCDataBufferShrdPtr();
                    output_line.append(CHAOS_FORMAT("%1%=%2% ", %*it%Base64Util::encode(buf->getBuffer(), buf->getBufferSize())));
                }
            }
            std::cout << node_uid << " DPCK_DATASET_TYPE_OUTPUT " <<output_line<< std::endl;
            
            break;
        }
            
        default:
            break;
    }
}

void NodeMonitorHandlerTest::noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type) {
    
}
