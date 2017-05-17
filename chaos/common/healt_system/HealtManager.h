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
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/healt_system/HealtMetric.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <chaos/common/chaos_types.h>

#include <boost/thread.hpp>

#include <sys/resource.h>

namespace chaos {
    namespace common{
        namespace healt_system {
            
            class SendHealthStatAsyncJob:
            public chaos::common::async_central::AsyncRunnable {

            protected:
                void run();
            public:
                SendHealthStatAsyncJob(chaos::common::data::CDataWrapper& health_stat);
                ~SendHealthStatAsyncJob();
            };
            
	  //retry for 12h
#define HELLO_PHASE_RETRY            12*3600
            struct ProcInfo {
                double usr_time;
                double sys_time;
                double swap_rsrc;
                
                ProcInfo():
                usr_time(0.0),
                sys_time(0.0),
                swap_rsrc(0.0){}
                
                ProcInfo& operator=(ProcInfo const &rhs) {
                    usr_time = rhs.usr_time;
                    sys_time = rhs.sys_time;
                    swap_rsrc = rhs.swap_rsrc;
                    return *this;
                };
            };
            
            //! define the map for the metric
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<HealtMetric>, HealtNodeElementMap)
            
            struct NodeHealtSet {
                //!notify when some metric has chagned
                bool    has_changed;
                
                //!save the time when the helat node has been created
                uint64_t startup_time;
                
                //!is the node uinique id
                std::string   node_uid;
                
                //!the key to use for the node publishing operation
                std::string   node_publish_key;
                
                //!is the metric node map
                HealtNodeElementMap map_metric;
                
                //!permit to lock the intere set
                boost::shared_mutex mutex_metric_set;
                
                //!keep track of how is the start valu eof the counter
                unsigned int fire_slot;
                
                NodeHealtSet(const std::string& _node_uid):
                has_changed(false),
                node_uid(_node_uid),
                node_publish_key(node_uid + chaos::NodeHealtDefinitionKey::HEALT_KEY_POSTFIX),
                fire_slot(0),
                startup_time(chaos::common::utility::TimingUtil::getTimeStamp()){}
                
                ~NodeHealtSet() {
                    //!clear all metric
                    map_metric.clear();
                }
            };
            
            //! define map for node health information
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<NodeHealtSet>, HealtNodeMap)
            
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
                //! counter for positioning new node healt set into the right fire slot
                unsigned int last_fire_counter_set;
                //!incremented at every timer timeout modded with HEALT_FIRE_SLOTS the result is the slot to fire
                unsigned int current_fire_slot;
                
                //! the map of the nodes healt
                HealtNodeMap                                        map_node;
                boost::shared_mutex                                 map_node_mutex;
                
                //! network broker and channel for comunicate with mds
                chaos::common::message::MultiAddressMessageChannel  *mds_message_channel;
                
                //! permit to lock the access to publishing direct io channel
                boost::mutex                                        mutex_publishing;
                
                //! drive rfor publishing the data
                ChaosUniquePtr<chaos::common::io::IODataDriver>      io_data_driver;
                
                //! private non locked push method for a healt set
                inline void _publish(const ChaosSharedPtr<NodeHealtSet>& heath_set,
                                     uint64_t publish_ts);
                
                ProcInfo current_proc_info;
                //last sampling resurce usage
                double last_usr_time;
                double last_sys_time;
                double last_total_time;
                uint64_t last_sampling_time;
                //! update the information about process
                inline void updateProcInfo();
            protected:
                //! default constructor and destructor
                HealtManager();
                ~HealtManager();
                
                //! timer handler for check what slot needs to be fired
                void timeout();
                chaos::common::data::CDataWrapper* prepareNodeDataPack(NodeHealtSet& node_health_set,
                                                                       uint64_t push_timestamp);
                
                //!protected mehoto to talk with mds to receive the cds server where publish the data
                int sayHello() throw (chaos::CException);
            public:
                //! inherited method
                void init(void *init_data) throw (chaos::CException);
                
                //! inherited method
                void start() throw (chaos::CException);
                
                //! inherited method
                void stop() throw (chaos::CException);
                
                //! inherited method
                void deinit() throw (chaos::CException);
                
                //! return the const info for the calculater proc info state
                const ProcInfo& getLastProcInfo();
                
                //!add a new node to the healt system
                void addNewNode(const std::string& node_uid);
                
                //!remove a node from the heatl system
                void removeNode(const std::string& node_uid);
                
                //! add a new metric to the already registered node
                void addNodeMetric(const std::string& node_uid,
                                   const std::string& node_metric,
                                   chaos::DataType::DataType metric_type);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param int32_value is the new integer value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        int32_t int32_value,
                                        bool publish = false);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param int64_value is the new integer value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        int64_t int64_value,
                                        bool publish = false);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param double_value is the new double value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        double double_value,
                                        
                                        bool publish = false);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param str_value is the new string value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const std::string& str_value,
                                        bool publish = false);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param str_value is the new string value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const char * c_str_value,
                                        bool publish = false);
                
                //! update the value for a metric
                /*!
                 \param node_uid node for wich we need to update the metric value
                 \param node_metric identify the metric to update
                 \param bool_value is the new bool value of the metric
                 \param publish, if true the node is publish after metris is updated
                 */
                void addNodeMetricValue(const std::string& node_uid,
                                        const std::string& node_metric,
                                        const bool bool_value,
                                        bool publish = false);
                
                //!publish health information for a node
                /*!
                 \param node_uid the node identification id for which we need to
                                    publish the healt data.
                 */
                void publishNodeHealt(const std::string& node_uid);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__HealtManager__) */
