/*
 *	NodeMonitor.h
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

#ifndef __CHAOSFramework__NodeMonitor_h
#define __CHAOSFramework__NodeMonitor_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxyManager.h>
#include <ChaosMetadataServiceClient/node_monitor/NodeController.h>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>
#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/node_monitor/node_monitor_types.h>
#include <ChaosMetadataServiceClient/node_monitor/NodeFetcher.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace metadata_service_client {
        //! forward declaration
        class ChaosMetadataServiceClient;
        
        
        //! name space that enclose all utility for the centralize
        //! node monitor funcitonality
        namespace node_monitor {
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<NodeFetcher>, NodeFetcherMap)

            class NodeMonitor:
            public chaos::common::utility::InizializableService {
                //! library settings
                ClientSetting *setting;
                friend class chaos::common::utility::InizializableServiceContainer<NodeMonitor>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;
                
                chaos::metadata_service_client::api_proxy::ApiProxyManager *api_proxy_manager;
                chaos::metadata_service_client::monitor_system::MonitorManager *monitor_manager;
                
                
                boost::mutex map_fetcher_mutex;
                NodeFetcherMap map_fetcher;
                
                
            protected:
                NodeMonitor(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                            chaos::metadata_service_client::api_proxy::ApiProxyManager *_api_proxy_manager,
                            chaos::metadata_service_client::ClientSetting *_setting);
                ~NodeMonitor();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                void startNodeMonitor(const std::string& node_uid,
                                      ControllerType controller_type);
                
                void stopNodeMonitor(const std::string& node_uid,
                                     ControllerType controller_type);
            public:

                bool addHandlerToNodeMonitor(const std::string& node_uid,
                                             ControllerType controller_type,
                                             NodeMonitorHandler *handler_to_add);
                
                bool removeHandlerToNodeMonitor(const std::string& node_uid,
                                                ControllerType controller_type,
                                                NodeMonitorHandler *handler_to_remove);
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeMonitor_h */
