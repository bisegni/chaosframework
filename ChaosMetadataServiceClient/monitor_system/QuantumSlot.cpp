/*
 *	QuantumSlot.cpp
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

#include <ChaosMetadataServiceClient/monitor_system/QuantumSlot.h>

#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>

#define QS_INFO   INFO_LOG (QuantumSlot)
#define QS_DBG    DBG_LOG  (QuantumSlot)
#define QS_ERR    ERR_LOG  (QuantumSlot)

using namespace chaos::common::utility;
using namespace chaos::metadata_service_client::monitor_system;

QuantumSlot::QuantumSlot(const std::string& _key,
                         int  _quantum_multiplier):
key(_key),
quantum_multiplier(_quantum_multiplier),
real_quantum(quantum_multiplier * MONITOR_QUANTUM_LENGTH),
consumers_priority_index(boost::multi_index::get<priority_index>(consumers)),
consumers_pointer_index(boost::multi_index::get<pointer_index>(consumers)),
last_processed_time(0),
last_send_data_duration(0),
send_data_iteration(0){}

QuantumSlot::~QuantumSlot() {}

const std::string& QuantumSlot::getKey() const {
    return key;
}

int QuantumSlot::getQuantumMultiplier() const {
    return quantum_multiplier;
}

void QuantumSlot::addNewConsumer(QuantumSlotConsumer *_consumer,
                                 unsigned int priotiy) {
    if(_consumer == NULL) return;
    consumers.insert(ConsumerType(_consumer, priority));
}

void QuantumSlot::removeConsumer(QuantumSlotConsumer *_consumer) {
    if(_consumer == NULL) return;
    uintptr_t pointer_int = reinterpret_cast<uintptr_t>(_consumer);
    SetConsumerTypePointerIndexIterator iter = consumers_pointer_index.find(pointer_int);
    if (iter != consumers_pointer_index.end()) {
        consumers_pointer_index.erase(pointer_int);
    }
}

void QuantumSlot::sendNewValueConsumer(const KeyValue& value) {
    uint64_t start_forwardint_time = TimingUtil::getTimeStampInMicrosends();
    for (SetConsumerTypePriorityIndexIterator it = consumers_priority_index.begin();
         it != consumers_priority_index.end();
         it++) {
        //signal the consumer
        reinterpret_cast<QuantumSlotConsumer*>(it->consumer_pointer)->quantumSlotHasData(key, value);
    }
    
    //calc time
    last_send_data_duration =  (TimingUtil::getTimeStampInMicrosends() - start_forwardint_time);
    if((send_data_iteration++ % 100) == 0) {
        //print statistic
        if(send_data_iteration >= 100) {
            QS_INFO << boost::str(boost::format("Forwarding processing time is %1% microseconds for %2% elements for key %3%_%4%")%
                                  ((double)last_send_data_duration/10)%
                                  consumers_priority_index.size()%
                                  key%
                                  quantum_multiplier);
        }
    }
}

//! set the last processing timestamp
void QuantumSlot::setLastProcessingTime(uint64_t _last_processing_time) {
    uint64_t time_diff = _last_processing_time - last_processed_time;
    last_processed_time = _last_processing_time;
    //if timediff is minor of real quantum the fetch delay is good
    quantum_is_good = time_diff<real_quantum;
}

//! determinate if the current quantum is good

bool QuantumSlot::isQuantumGood() {
    return quantum_is_good;
}