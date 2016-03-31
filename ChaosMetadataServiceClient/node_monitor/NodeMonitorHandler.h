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
            
            struct NodeMonitorHandlerComparator;
            
            //! node monitor handler
            class NodeMonitorHandler {
                friend class NodeMonitorHandlerComparator;
                const std::string handler_uuid;
            public:
                NodeMonitorHandler();
                virtual ~NodeMonitorHandler();
                
                //! called when an online state has changed
                virtual void nodeChangedOnlineStatus(const std::string& node_uid,
                                                     OnlineStatus old_status,
                                                     OnlineStatus new_status) = 0;
                
                virtual void nodeChangedProcessResource(const std::string& node_uid,
                                                        const ProcessResource& old_proc_res,
                                                        const ProcessResource& new_proc_res) = 0;
                
                virtual void nodeChangedErrorInformation(const std::string& node_uid,
                                                         const ErrorInformation& old_status,
                                                         const ErrorInformation& new_status) = 0;
                
                void datasetHasBeenUpdated(const std::string& node_uid);
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
