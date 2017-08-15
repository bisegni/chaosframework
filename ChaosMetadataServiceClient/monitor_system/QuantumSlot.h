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

#ifndef __CHAOSFramework__QuantumSlot__
#define __CHAOSFramework__QuantumSlot__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/thread.hpp>

#include <string>
#include <set>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            
            //! forward declaration
            class QuantumSlotScheduler;
            
            struct ConsumerType {
                uint32_t    priority;
                uintptr_t   consumer_pointer;
                
                ConsumerType(QuantumSlotConsumer *_consumer,
                             unsigned int _priority):
                priority(_priority),
                consumer_pointer(reinterpret_cast<uintptr_t>(_consumer)){}
                
                bool operator<(const ConsumerType& ct)const{return priority<ct.priority;}
            };
            
            //multi-index class
            struct pointer_index{};
            struct priority_index{};
            
            //multi-index set
            typedef boost::multi_index_container<
            ConsumerType,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<boost::multi_index::tag<priority_index>,  BOOST_MULTI_INDEX_MEMBER(ConsumerType, uint32_t, priority)>,
            boost::multi_index::ordered_unique<boost::multi_index::tag<pointer_index>,  BOOST_MULTI_INDEX_MEMBER(ConsumerType, uintptr_t, consumer_pointer)>
            >
            > SetConsumerType;
            
            //!priority index and iterator
            typedef boost::multi_index::index<SetConsumerType, priority_index>::type                      SetConsumerTypePriorityIndex;
            typedef boost::multi_index::index<SetConsumerType, priority_index>::type::iterator            SetConsumerTypePriorityIndexIterator;
            typedef boost::multi_index::index<SetConsumerType, priority_index>::type::reverse_iterator    SetConsumerTypePriorityIndexReverseIterator;
            
            //!priority index and iterator
            typedef boost::multi_index::index<SetConsumerType, pointer_index>::type                       SetConsumerTypePointerIndex;
            typedef boost::multi_index::index<SetConsumerType, pointer_index>::type::iterator             SetConsumerTypePointerIndexIterator;
            typedef boost::multi_index::index<SetConsumerType, pointer_index>::type::reverse_iterator     SetConsumerTypePointerIndexReverseIterator;

            
            //! define the descirption for a quantum slot for the fetcher
            /*!
             the current_quantum_slot is decremented every time the scheduler
             scans the description every QUANTUM minimum delay. When it is 0(or minor)
             the the slot is forwarded to the queue for the update of the value
             */
            class QuantumSlot {
                friend class QuantumSlotScheduler;
                //is the key for wich we need to retrive the value
                const std::string   key;
                
                //! is the multiplier for the quantum, when it is 0 ti is schedule for execution
                const int           quantum_multiplier;
                
                //! is the real quantum calculated by quantum_multiplier*MONITOR_QUANTUM_LENGTH
                uint64_t            real_quantum;
                
                //! priority
                int                 priority;
                
                //! is the last processing time for this slot
                uint64_t            last_processed_time;
                
                //! determinate i fthe quantum processing delay is good
                bool                quantum_is_good;
                
                //! identify a purgeable slot
                bool                purgeable;
                
                //multi-index structure for managment of consumer
                boost::shared_mutex              consumer_mutex;
                SetConsumerType                  consumers;
                SetConsumerTypePriorityIndex&    consumers_priority_index;
                SetConsumerTypePointerIndex&     consumers_pointer_index;
                
                //statistic variable
                uint64_t last_send_data_duration;
                int send_data_iteration;
                
                QuantumSlot(const std::string& _key,
                            int  _quantum_multiplier);
                
                void sendNewValueConsumer(const KeyValue& _value);
                void sendNoValueToConsumer();
            public:
                ~QuantumSlot();
                
                //! set the last processing timestamp
                void setLastProcessingTime(uint64_t last_processing_time);
                //! determinate if the current quantum is good
                /*!
                 this function calculate if the fetch of the value is contained
                 whithin the quantum(* multiplier). If this function return false
                 the problem is that the slot has been executed out of his quantum
                 */
                bool isQuantumGood();
                
                const std::string& getKey() const;
                int getQuantumMultiplier() const;
                
                void addNewConsumer(QuantumSlotConsumer *_consumer,
                                    unsigned int priotiy);
                bool removeConsumer(QuantumSlotConsumer *_consumer);

                int32_t size();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__QuantumSlot__) */
