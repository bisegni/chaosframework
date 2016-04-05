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
public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    const std::string node_uid;
protected:
    //! called when an online state has changed
    void nodeChangedOnlineState(const std::string& node_uid,
                                 chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                 chaos::metadata_service_client::node_monitor::OnlineState new_status);
    
    void nodeChangedInternalState(const std::string& node_uid,
                                  const std::string& old_status,
                                  const std::string& new_status);
    
    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);
    
    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& old_status,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& new_status);
    
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);
    
    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);
    
    //--------registration handler
    void handlerHasBeenRegistered(const std::string& node_uid,
                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_status);
    void handlerHasBeenRegistered(const std::string& control_unit_uid,
                                  int dataset_type,
                                  chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);
    
public:
    NodeMonitorHandlerTest(const std::string& _node_uid);
    ~NodeMonitorHandlerTest();
};

#endif /* NodeMonitorHandlerTest_hpp */
