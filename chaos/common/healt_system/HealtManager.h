/*
 *	HealtManager.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__HealtManager__
#define __CHAOSFramework__HealtManager__
#include <chaos/common/chaos_constants.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/healt_system/HealtMetric.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <chaos/common/chaos_types.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common{
        namespace healt_system {

            //! define the map for the metric
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr<HealtMetric>, HealtNodeElementMap)

            struct NodeHealtSet {
                    //notify when some metric has chagned
                bool    has_changed;
                    //the key to use for the node publishing operation
                std::string   node_key;

                    //is the metric node map
                HealtNodeElementMap map_metric;

                    //!permit to lock the intere set
                boost::shared_mutex mutex_metric_set;

                    //!keep track of how is the start valu eof the counter
                unsigned int fire_counter_configured;
                
                    //!we it reach 0 the metric is published
                int fire_counter;
                
                NodeHealtSet(const std::string& node_uid):
                has_changed(false),
                node_key(node_uid + chaos::NodeHealtDefinitionKey::HEALT_KEY_POSTFIX) {}
            };

            //! define map for node health information
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr<NodeHealtSet>, HealtNodeMap)

                //! Is the root class for the healt managment system
            /*!
             !CHAOS helat system consits in a set of information, about nodes, published to the central data service
             or requested by other node.
             Every Node within a process can register itself on helat managment and request to update standard value
             or custom one. These infromation are memoryzed in key taht is a composition between the ndk_uid and the
             postfix '_healt'.
             The time for auto push is set to one seconds and the handler decrement, at each fire, the fire_counter of
             the NodeHealtSet Structure. when the counter reach the 0 it the healt set is published and the counter
             is reset to a new random value
             */
            class HealtManager:
            public chaos::common::async_central::TimerHandler,
            public chaos::common::utility::Singleton<HealtManager>,
            public chaos::common::utility::StartableService {
                friend class chaos::common::utility::Singleton<HealtManager>;
                
                unsigned int last_fire_counter_set;
                
                HealtNodeMap                                        map_node;
                boost::shared_mutex                                 map_node_mutex;

                chaos::common::network::NetworkBroker               *network_broker_ptr;
                chaos::common::message::MultiAddressMessageChannel  *mds_message_channel;
                
                boost::mutex                                        mutex_publishing;
                std::auto_ptr<chaos::common::io::IODataDriver>      io_data_driver;
                
                inline void _publish(const boost::shared_ptr<NodeHealtSet>& heath_set);
            protected:
                HealtManager();
                ~HealtManager();
                void timeout();
                chaos::common::data::CDataWrapper* prepareNodeDataPack(HealtNodeElementMap& element_map,
                                                                       uint64_t push_timestamp);
                void sayHello() throw (chaos::CException);
            public:
                void init(void *init_data) throw (chaos::CException);
                void start() throw (chaos::CException);
                void stop() throw (chaos::CException);
                void deinit() throw (chaos::CException);

                    //s
                void setNetworkBroker(chaos::common::network::NetworkBroker *_network_broker);

                void addNewNode(const std::string& node_uid);
                
                void removeNode(const std::string& node_uid);
                
                void addNodeMetric(const std::string& node_uid,
                                   const std::string& node_metric,
                                   chaos::DataType::DataType metric_type);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        int32_t int32_value,
                                        bool publish = false);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        int64_t int64_value,
                                        bool publish = false);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        double double_value,
                                        bool publish = false);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const std::string& str_value,
                                        bool publish = false);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const char * c_str_value,
                                        bool publish = false);
                
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const bool bool_value,
                                        bool publish = false);

                    //publish the healt for the ndoe uid
                void publishNodeHealt(const std::string& node_uid);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__HealtManager__) */
