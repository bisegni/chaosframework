/*
 *	monitor_system_types.h
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

#ifndef CHAOSFramework_monitor_system_types_h
#define CHAOSFramework_monitor_system_types_h

#include <chaos/common/data/CDataWrapper.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            
            class QuantumSlot;
            class QuantumSlotScheduler;
            
            //!is the minimal monitoring delay
#define MONITOR_QUANTUM_LENGTH  100//milli-seconds
#define CHAOS_QSS_COMPOSE_QUANTUM_CONSUMER_KEY(k,q,p) boost::str(boost::format("%1%_%2%_%3%")% k % q % reinterpret_cast<uintptr_t>(p))
#define CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(k,q) boost::str(boost::format("%1%_%2%")% k % q)
            
            typedef boost::shared_ptr<chaos::common::data::CDataWrapper> KeyValue;
            
            //! Base class for slot consumer
            class QuantumSlotConsumer {
                friend class QuantumSlot;
                friend class QuantumSlotScheduler;
                //!mutex and variable that help to notify when the consumer is free to be relased
                //!and no more used by scheduler internal layer
                bool                                    free_of_work;
                boost::mutex                            mutex_condition_free;
                boost::condition_variable               condition_free;
                //!keep track of how many layers of the  scheduler are using this slot
                boost::atomic<unsigned int> usage_counter;
                //!relase any lock on conditional interal variable
                void setFreeOfWork() {
                    try{
                        boost::mutex::scoped_lock lock_on_condition(mutex_condition_free);
                        free_of_work = true;
                        condition_free.notify_one();
                    }catch(...){}
                }
            public:
                QuantumSlotConsumer():
                usage_counter(0),
                free_of_work(false){}
                
                virtual ~QuantumSlotConsumer() {}
                
                //!as called every time there is new data for quantum slot
                virtual void quantumSlotHasData(const std::string& key, const KeyValue& value) = 0;
                //! callend every time that data can't be retrieved from data service
                virtual void quantumSlotHasNoData(const std::string& key) = 0;
                //! waith on conditional interval variable that is fired when the consumer can be released
                void waitForCompletition() {
                    try{
                        boost::mutex::scoped_lock lock_on_condition(mutex_condition_free);
                        while(!free_of_work) {
                            condition_free.wait(lock_on_condition);
                        }
                        free_of_work = false;
                    }catch(...){}
                }
            };
        }
    }
}

#endif
