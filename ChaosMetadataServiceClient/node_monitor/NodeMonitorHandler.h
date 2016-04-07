/*
 *	NodeMonitorHandler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 29/03/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__NodeMonitorHandler_h
#define __CHAOSFramework__NodeMonitorHandler_h

#include <ChaosMetadataServiceClient/node_monitor/node_monitor_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            class NodeFetcher;
            struct NodeMonitorHandlerComparator;
            
            //! node monitor handler
            class NodeMonitorHandler {
                friend class NodeMonitorHandlerComparator;
                friend class NodeFetcher;
                const std::string handler_uuid;
            public:
                static const char * const MAP_KEY_ONLINE_STATE;
                NodeMonitorHandler();
                virtual ~NodeMonitorHandler();
                
                //! called when an online state has changed
                virtual void nodeChangedOnlineState(const std::string& node_uid,
                                                    OnlineState old_state,
                                                    OnlineState new_state);
                
                virtual void nodeChangedInternalState(const std::string& node_uid,
                                                      const std::string& old_state,
                                                      const std::string& new_state);
                
                virtual void nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res);
                
                virtual void nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_error_information,
                                                         const ErrorInformation& new_error_information);
                
                virtual void nodeChangedHealthDataset(const std::string& node_uid,
                                                      MapDatasetKeyValues& map_health_dataset);
                
                virtual void handlerHasBeenRegistered(const std::string& node_uid,
                                                      const HealthInformation& current_health_state,
                                                      MapDatasetKeyValues& map_health_dataset);
            };
            
            struct NodeMonitorHandlerComparator {
                bool operator() (NodeMonitorHandler* h1, NodeMonitorHandler* h2) {
                    return h1->handler_uuid.compare(h2->handler_uuid) < 0;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeMonitorHandler_h */
