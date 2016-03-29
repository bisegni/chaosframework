/*
 *	NodeController.h
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

#ifndef __CHAOSFramework__NodeController_h
#define __CHAOSFramework__NodeController_h

#include <chaos/common/chaos_types.h>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>
#include <ChaosMetadataServiceClient/node_monitor/NodeMonitorHandler.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            //! forward declaration
            class NodeMonitor;
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, MonitorKeyList)
            
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator> MonitoHandlerList;
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator>::iterator MonitoHandlerListIterator;
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator>::const_iterator MonitoHandlerListConstIterator;
            
            class NodeController:
            public chaos::metadata_service_client::monitor_system::QuantumSlotConsumer {
                friend class NodeMonitor;
                
                const std::string node_uid;
                const std::string node_health_uid;
                
                //the list of all registered handlers
                boost::mutex        list_handler_mutex;
                MonitoHandlerList   list_handler;
                
                //count the time we have got the same timestamp
                bool was_online;
                unsigned int zero_diff_count_on_ts;
                uint64_t last_recevied_ts;
                std::string last_received_status;
                
                inline void _resetHealth();
                inline void _setOnlineStatus(const OnlineStatus new_online_status);
                inline void _setError(const ErrorInformation& new_error_information);
                inline void _setProcessResource(const ProcessResource& new_process_resource);
            protected:
                
                HealthInformation health_info;
                
                MonitorKeyList monitor_key_list;
                
                NodeController(const std::string& _node_uid);

                //!inherited method
                void quantumSlotHasData(const std::string& key,
                                        const chaos::metadata_service_client::monitor_system::KeyValue& value);
                //!inherited method
                void quantumSlotHasNoData(const std::string& key);
            public:
                
                virtual ~NodeController();
                
                const std::string& getNodeUID();
                
                const MonitorKeyList& getMonitorKeyList() const;
                
                const MonitoHandlerList& getMonitorHandlerList() const;
                
                const HealthInformation& getHealthInformation() const;
                
                bool addHandler(NodeMonitorHandler *handler_to_add);
                
                bool removeHandler(NodeMonitorHandler *handler_to_remove);
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeController_h */
