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

#ifndef __CHAOSFramework__QuantumSlotScheduler__
#define __CHAOSFramework__QuantumSlotScheduler__

#include <chaos_metadata_service_client/monitor_system/monitor_system_types.h>
#include <chaos_metadata_service_client/monitor_system/QuantumSlot.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/asio.hpp>

#include <vector>
#include <string>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            //! forward declaration
            class MonitorManager;
            
            //define a map for string and quantum slot
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, QuantumSlotConsumer*, QuantumSlotConsumerMap)
            
#define LOCK_QUEUE(queue_name)\
boost::unique_lock<boost::mutex> wl(mutex_ ## queue_name);
    
#define UNLOCK_QUEUE(queue_name)\
mutex_ ## queue_name.unlock();
            
            //! structure used to submit new consumer to itnernal layer
            struct SlotConsumerInfo {
                //! operation for this consumer add or remove to quantum slot
                bool operation;
                //!key to monitor
                std::string key_to_monitor;
                //!the requested multiplier for this consumer
                unsigned int quantum_multiplier;
                //! the requested priority for this consumer
                unsigned int consumer_priority;
                //!slot to call
                QuantumSlotConsumer *consumer;
                
                //!default constructor
                SlotConsumerInfo(bool _operation,
                                 const std::string& _key_to_monitor,
                                 unsigned int _quantum_multiplier,
                                 QuantumSlotConsumer *_consumer,
                                 unsigned int _consumer_priority):
                operation(_operation),
                key_to_monitor(_key_to_monitor),
                quantum_multiplier(_quantum_multiplier),
                consumer(_consumer),
                consumer_priority(_consumer_priority){}
                
                //! copy constructor
                SlotConsumerInfo(SlotConsumerInfo& _info):
                operation(_info.operation),
                key_to_monitor(_info.key_to_monitor),
                quantum_multiplier(_info.quantum_multiplier),
                consumer(_info.consumer),
                consumer_priority(_info.consumer_priority){}
                
            };
            
            //! strucutre that contain the slot
            struct ScheduleSlot {
                ChaosSharedPtr<QuantumSlot> quantum_slot;
                //! indeitfy the current slot, when is 0 it will be executed
                int current_quantum;
                //! key composed by the key and the quantum 'key_quantum'
                const std::string quantum_slot_key;
                ScheduleSlot(ChaosSharedPtr<QuantumSlot>& _quantum_slot):
                quantum_slot(_quantum_slot),
                current_quantum(_quantum_slot->getQuantumMultiplier()),
                quantum_slot_key(CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(_quantum_slot->getKey(), current_quantum)){}
                
                bool operator<(const ScheduleSlot& ss)const{return current_quantum<ss.current_quantum;}
            };
            
            //multi-index class
            struct ss_current_quantum{};
            struct ss_quantum_slot_key{};
            
            //multi-index set
            typedef boost::multi_index_container<
            ScheduleSlot,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<boost::multi_index::tag<ss_current_quantum>,  BOOST_MULTI_INDEX_MEMBER(ScheduleSlot, int, current_quantum)>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<ss_quantum_slot_key>,  BOOST_MULTI_INDEX_MEMBER(ScheduleSlot, const std::string, quantum_slot_key)>
            >
            > SetScheduleSlot;
            
            //!priority index and iterator
            typedef boost::multi_index::index<SetScheduleSlot, ss_current_quantum>::type                      SSSlotTypeCurrentQuantumIndex;
            typedef boost::multi_index::index<SetScheduleSlot, ss_current_quantum>::type::iterator            SSSlotTypeCurrentQuantumIndexIterator;
            typedef boost::multi_index::index<SetScheduleSlot, ss_current_quantum>::type::reverse_iterator    SSSlotTypeCurrentQuantumIndexReverseIterator;
            
            //!priority index and iterator
            typedef boost::multi_index::index<SetScheduleSlot, ss_quantum_slot_key>::type                     SSSlotTypeQuantumSlotKeyIndex;
            typedef boost::multi_index::index<SetScheduleSlot, ss_quantum_slot_key>::type::iterator           SSSlotTypeQuantumSlotKeyIndexIterator;
            
            //!multi-index quantum decrement function
            struct ScheduleSlotDecrementQuantum
            {
                ScheduleSlotDecrementQuantum(){}
                
                void operator()(ScheduleSlot& ss) {
                    if(ss.current_quantum > 0) {
                        ss.current_quantum--;
                    }
                }
            };
            
            struct ScheduleSlotResetQuantum
            {
                ScheduleSlotResetQuantum(){}
                
                void operator()(ScheduleSlot& ss) {
                    ss.current_quantum  = ss.quantum_slot->getQuantumMultiplier();
                }
            };
            
            struct ScheduleSlotDisableQuantum
            {
                ScheduleSlotDisableQuantum(){}
                
                void operator()(ScheduleSlot& ss) {
                    ss.current_quantum = -1;
                }
            };
            
            typedef std::queue<SlotConsumerInfo*>  QueueSlotConsumerInfo;
            typedef boost::lockfree::queue<QuantumSlot*, boost::lockfree::fixed_sized<false> >  LFQuantumSlotQueue;
            
            //! class that manage the scan of the slot ad the fetch of the slot vlaue
            /*!
             This scheduler consists into two job, the scanner and the fetcher.
             The scanner, is the job that scans all available quantum slot and, for each one,
             decrement his current quantum. Qhen it goes reach( or goes below, the 0 the slot
             is put withing the queue that colelct all slot that nened to have his value refresched.
             The Fetcher si the job that scan the active slot queue and for each one fetch the
             value form the cds.
             */
            class QuantumSlotScheduler:
            public chaos::common::utility::StartableService {
                friend class MonitorManager;
                std::vector<std::string>                data_driver_endpoint;
                chaos::common::network::NetworkBroker   *network_broker;
                std::string                             data_driver_impl;
                
                boost::mutex                            mutex_condition_fetch;
                boost::condition_variable               condition_fetch;
                
                boost::mutex                            mutex_condition_scan;
                boost::condition_variable               condition_scan;
                
                //!groups for thread that make the scanner
                bool                                    work_on_scan;
                ChaosSharedPtr<boost::thread_group>  scanner_threads;
                
                //!groups for thread that fetch the value for a key
                bool                                    work_on_fetch;
                ChaosSharedPtr<boost::thread_group>  fetcher_threads;
                
                //! queue for all the active slot
                LFQuantumSlotQueue                      queue_active_slot;
                
                //!multi index set for the slot used internal only
                SetScheduleSlot                         set_slots;
                SSSlotTypeCurrentQuantumIndex&          set_slots_index_quantum;
                SSSlotTypeQuantumSlotKeyIndex&          set_slots_index_key_slot;
                
                //! manage the lock on the slot multi-index
                boost::shared_mutex                     set_slots_mutex;
                
                //------------structure for comunication between public and internal layers-------------------------------------------
                //! queue that conenct the public and internal layers of scheduler add and remove handler push quantum slot
                //! withing this queue and scan slot funciton retrive new one added and increment the multiindex set
                boost::mutex                          mutex_queue_new_quantum_slot_consumer;
                QueueSlotConsumerInfo                 queue_new_quantum_slot_consumer;
                
                //!mute for work on map that of slot consumer managed by add and remove function
                boost::mutex                            mutex_map_quantum_slot_consumer;
                
                
                //! default constructor
                QuantumSlotScheduler(chaos::common::network::NetworkBroker *_network_broker);
                ~QuantumSlotScheduler();
                
                //! Scan the slot to find which of that can be executed
                /*!
                 the trhead wait exatly the minimum quantum after that it respawn and
                 check the slot decrementing the current_quantum of each one
                 */
                void scanSlot();
                
                //! fetch the value for all the slot within the queue_active_slot queue
                void fetchValue(ChaosSharedPtr<chaos::common::io::IODataDriver> data_driver);
                
                //! add a new thread to the fetcher job
                void addNewfetcherThread();
                
                void dispath_new_value_async(const boost::system::error_code& error,
                                             QuantumSlot *cur_slot,
                                             char *data_found);
                //! manage the registration in internal layer for new consumer
                void _addKeyConsumer(SlotConsumerInfo *ci);
                //!manage in the internal layer the request for remove the consumer
                bool _removeKeyConsumer(SlotConsumerInfo *ci);
                
                //!check the internal queue if there are new consumer to add
                inline uint64_t _checkRemoveAndAddNewConsumer(uint64_t start_time_in_milliseconds,
                                                              uint64_t millisecond_for_work);
            public:
                
                void init(void *init_data) throw (chaos::CException);
                void start() throw (chaos::CException);
                void stop() throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                //! Set a new lits of server to use for fetch values
                void setDataDriverEndpoints(const std::vector<std::string>& _data_driver_endpoint);
                
                //! Add a new quantum slot for key
                void addKeyConsumer(const std::string& key_to_monitor,
                                    unsigned int quantum_multiplier,
                                    QuantumSlotConsumer *consumer,
                                    unsigned int consumer_priority = 500);
                
                //! Remove a consumer by key and quantum
                /*!
                 The remove operation can be also executed specifind false on
                 wait_completion parameter. In this case The scheduler try to
                 remove the consumer but if it is in use, the remove operation
                 will be submitted to the asynchronous layer, so the caller 
                 neet to call the waitForCompletion of the consumer to
                 waith that the remove operationhas been terminated
                 \param key_to_monitor the key to monitor
                 \param quantum_multiplier is the quantum multipier that will determinate
                        the delay form a data request and nother
                 \param consumer the pointer of the consumer that need to be notified
                 \param wait_completion detarminate if the called whant to wait the completion
                        of the operation or whant check by itself
                 \return true if the consumer has been removed
                 */
                bool removeKeyConsumer(const std::string& key_to_monitor,
                                       unsigned int quantum_multiplier,
                                       QuantumSlotConsumer *consumer,
                                       bool wait_completion = true);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__QuantumSlotScheduler__) */
