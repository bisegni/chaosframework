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

#ifndef __CHAOSFramework__NodeController_h
#define __CHAOSFramework__NodeController_h

#include <chaos/common/chaos_types.h>

#include <chaos_metadata_service_client/monitor_system/monitor_system_types.h>
#include <chaos_metadata_service_client/node_monitor/NodeMonitorHandler.h>

#include <boost/thread.hpp>

#define CHECK_DS_CHANGED(x, v)\
((x.get() == NULL) || \
((x.get() != NULL) && (x->toHash().compare(v->toHash()) != 0)))

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            //! forward declaration
            class NodeMonitor;
            class NodeFetcher;
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, MonitorKeyList)
            
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator> MonitoHandlerList;
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator>::iterator MonitoHandlerListIterator;
            typedef std::set<NodeMonitorHandler*, NodeMonitorHandlerComparator>::const_iterator MonitoHandlerListConstIterator;
            
            class NodeController {
                friend class NodeMonitor;
                friend class NodeFetcher;
                const std::string node_uid;
                const std::string node_health_uid;
                
                //count the time we have got the same timestamp
                bool was_online;
                unsigned int zero_diff_count_on_ts;
                uint64_t last_recevied_ts;
                std::string last_received_status;
               
                //!last dataset received for helth data
                MapDatasetKeyValues map_ds_health;
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_healt;
                
                inline void _resetHealth();
                inline void _setOnlineState(const OnlineState new_online_state);
                inline void _setNodeInternalState(const std::string& new_internal_state);
                inline void _setError(const ErrorInformation& new_error_information);
                inline void _setProcessResource(const ProcessResource& new_process_resource);
                inline void _fireHealthDatasetChanged();
                void updateData();
            protected:
                //the list of all registered handlers
                boost::mutex        list_handler_mutex;
                MonitoHandlerList   list_handler;
                
                HealthInformation health_info;
                
                MonitorKeyList monitor_key_list;
                
                NodeController(const std::string& _node_uid);

                //!inherited method
                virtual void quantumSlotHasData(const std::string& key,
                                        const chaos::metadata_service_client::monitor_system::KeyValue& value);
                //!inherited method
                virtual void quantumSlotHasNoData(const std::string& key);
            public:
                
                virtual ~NodeController();
                
                const std::string& getNodeUID();
                
                const unsigned int getHandlerListSise();
                
                const MonitorKeyList& getMonitorKeyList() const;
                
                const MonitoHandlerList& getMonitorHandlerList() const;
                
                const HealthInformation& getHealthInformation() const;
                
                virtual bool addHandler(NodeMonitorHandler *handler_to_add);
                
                bool removeHandler(NodeMonitorHandler *handler_to_remove);
                
                MapDatasetKeyValues& getHealthDataset();
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeController_h */
