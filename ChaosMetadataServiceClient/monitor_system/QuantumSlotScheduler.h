/*
 *	QuantumSlotScheduler.h
 *	!CHAOS
 *	Created by Bisegni Claudio & Sandro Angius
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
#ifndef __CHAOSFramework__QuantumSlotScheduler__
#define __CHAOSFramework__QuantumSlotScheduler__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>
#include <ChaosMetadataServiceClient/monitor_system/QuantumSlot.h>

#include <chaos/common/utility/StartableService.h>

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            //! forward declaration
            class MonitorManager;
            
#define CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(k,q) boost::str(boost::format("%1%_%2%")% k % q)
            
            typedef boost::lockfree::queue<QuantumSlot*, boost::lockfree::fixed_sized<false> >  ActiveQuantumSlotQueue;
            
            //! strucutre that contain the slot
            struct ScheduleSlot {
                boost::shared_ptr<QuantumSlot> quantum_slot;
                //! indeitfy the current slot, qhen is 0 it will be executed
                int current_quantum;
                //! key composed by the key and the quantum 'key_quantum'
                const std::string quantum_slot_key;
                ScheduleSlot(boost::shared_ptr<QuantumSlot>& _quantum_slot):
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
                    if(ss.current_quantum < 0) {
                        ss.current_quantum = ss.quantum_slot->getQuantumMultiplier();
                    } else {
                        ss.current_quantum--;
                    }
                }
            };
            
            class QuantumSlotScheduler:
            public chaos::common::utility::StartableService {
                friend class MonitorManager;
                
                boost::mutex                mutex_condition;
                boost::condition_variable   condition_fetch;
                
                //groups for thread that make the scanner
                bool                    work_on_scan;
                boost::thread_group     scanner_threads;
                
                //!groups for thread that fetch the value for a key
                bool                    work_on_fetch;
                boost::thread_group     fetcher_threads;
                
                ActiveQuantumSlotQueue  queue_active_slot;
                
                //multi index set for the slot
                SetScheduleSlot                  set_slots;
                SSSlotTypeCurrentQuantumIndex&   set_slots_index_quantum;
                SSSlotTypeQuantumSlotKeyIndex&   set_slots_index_key_slot;
                
                //manage the lock on the slot multi-index
                boost::shared_mutex              set_slots_mutex;
                
                QuantumSlotScheduler();
                ~QuantumSlotScheduler();
                
                //! Scan the slot to find which of that can be executed
                /*!
                 the trhead wait exatly the minimum quantum after that it respawn and 
                 check the slot decrementing the current_quantum of each one
                 */
                void scanSlot();
                
                //! fetch the value for all the slot within the queue_active_slot queue
                void fetchValue();
                
            public:
                
                void init(void *init_data) throw (chaos::CException);
                void start() throw (chaos::CException);
                void stop() throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                //! add a new quantum slot for key
                void addKeyConsumer(const std::string& key_to_monitor,
                                    int quantum_multiplier,
                                    QuantumSlotConsumer *consumer,
                                    int consumer_priority = 500);
                
                void removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__QuantumSlotScheduler__) */
