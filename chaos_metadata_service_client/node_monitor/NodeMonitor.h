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

#ifndef __CHAOSFramework__NodeMonitor_h
#define __CHAOSFramework__NodeMonitor_h

#include <chaos_metadata_service_client/api_proxy/ApiProxyManager.h>
#include <chaos_metadata_service_client/node_monitor/NodeController.h>
#include <chaos_metadata_service_client/monitor_system/monitor_system.h>
#include <chaos_metadata_service_client/node_monitor/NodeFetcher.h>

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
                friend class chaos::common::utility::InizializableServiceContainer<NodeMonitor>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;
                
                chaos::metadata_service_client::api_proxy::ApiProxyManager *api_proxy_manager;
                chaos::metadata_service_client::monitor_system::MonitorManager *monitor_manager;
                
                
                boost::mutex map_fetcher_mutex;
                NodeFetcherMap map_fetcher;
            protected:
                NodeMonitor(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                            chaos::metadata_service_client::api_proxy::ApiProxyManager *_api_proxy_manager);
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
