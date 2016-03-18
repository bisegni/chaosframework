//
//  NodeMonitorHandlerTest.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/16.
//  Copyright © 2016 INFN. All rights reserved.
//

#include "NodeMonitorHandlerTest.h"

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
    }
}

NodeMonitorHandlerTest::NodeMonitorHandlerTest(const std::string& _node_uid):
node_uid(_node_uid){
    ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid,
                                                                       this);
}

NodeMonitorHandlerTest::~NodeMonitorHandlerTest() {
    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid,
                                                                          this);
}

using namespace chaos::metadata_service_client::node_monitor;

void NodeMonitorHandlerTest::nodeChangedOnlineStatus(const std::string& node_uid,
                                                     OnlineStatus old_status,
                                                     OnlineStatus new_status) {
    std::cout << "nodeChangedOnlineStatus: " << getStatusDesc(new_status)<<"["<<getStatusDesc(old_status)<<"]" << std::endl;
}

void NodeMonitorHandlerTest::nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res) {
    std::cout << "nodeChangedProcessResource: " << std::endl;
    
}

void NodeMonitorHandlerTest::nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_status,
                                                         const ErrorInformation& new_status) {
    std::cout << "nodeChangedErrorInformation: " << std::endl;
    
}