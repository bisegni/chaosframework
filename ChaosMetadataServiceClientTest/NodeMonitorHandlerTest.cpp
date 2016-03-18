//
//  NodeMonitorHandlerTest.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/16.
//  Copyright © 2016 INFN. All rights reserved.
//

#include "NodeMonitorHandlerTest.h"

using namespace chaos::metadata_service_client::node_monitor;

void NodeMonitorHandlerTest::nodeChangedOnlineStatus(const std::string& node_uid,
                                                 OnlineStatus old_status,
                                                 OnlineStatus new_status) {
    std::cout << "nodeChangedOnlineStatus: " << new_status<<"["<<old_status<<"]" << std::endl;
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