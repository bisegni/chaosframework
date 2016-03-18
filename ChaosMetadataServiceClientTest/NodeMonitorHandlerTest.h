//
//  NodeMonitorHandlerTest.hpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/16.
//  Copyright © 2016 INFN. All rights reserved.
//

#ifndef NodeMonitorHandlerTest_hpp
#define NodeMonitorHandlerTest_hpp

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class NodeMonitorHandlerTest:
public chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    //! called when an online state has changed
    void nodeChangedOnlineStatus(const std::string& node_uid,
                                 chaos::metadata_service_client::node_monitor::OnlineStatus old_status,
                                 chaos::metadata_service_client::node_monitor::OnlineStatus new_status);
    
    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);
    
    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& old_status,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& new_status);
};

#endif /* NodeMonitorHandlerTest_hpp */
