/*
 *	QuantumSlotScheduler.cpp
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

#include <ChaosMetadataServiceClient/monitor_system/QuantumSlotScheduler.h>

#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>

#define STAT_ITERATION 10

#define QSS_INFO   INFO_LOG(QuantumSlotScheduler)
#define QSS_DBG    INFO_LOG(QuantumSlotScheduler)
#define QSS_ERR    INFO_LOG(QuantumSlotScheduler)

using namespace chaos::common::utility;
using namespace chaos::metadata_service_client::monitor_system;

QuantumSlotScheduler::QuantumSlotScheduler():
queue_active_slot(100),
set_slots_index_quantum(boost::multi_index::get<ss_current_quantum>(set_slots)),
set_slots_index_key_slot(boost::multi_index::get<ss_quantum_slot_key>(set_slots)) {
    
}

QuantumSlotScheduler::~QuantumSlotScheduler() {
    
}


void QuantumSlotScheduler::init(void *init_data) throw (chaos::CException) {
}

void QuantumSlotScheduler::start() throw (chaos::CException) {
    work_on_scan = true;
    work_on_fetch = true;
    scanner_threads.add_thread(new boost::thread(bind(&QuantumSlotScheduler::scanSlot, this)));
    fetcher_threads.add_thread(new boost::thread(bind(&QuantumSlotScheduler::fetchValue, this)));
}

void QuantumSlotScheduler::stop() throw (chaos::CException) {
    work_on_scan = false;
    work_on_fetch = false;
    scanner_threads.join_all();
    fetcher_threads.join_all();

}

void QuantumSlotScheduler::deinit() throw (chaos::CException) {
    
}

void QuantumSlotScheduler::scanSlot() {
    int         stat_counter = 0;
    int64_t     milliseconds_to_sleep = 0;
    uint64_t    milliseconds_start = 0;
    uint64_t    current_processing_time = 0;
    ScheduleSlotDecrementQuantum decrement_quantum_op;
    
    while(work_on_scan) {
        milliseconds_start = TimingUtil::getTimeStamp();
        //set_urls_rb_pos_index.modify(it, URLServiceIndexUpdateRBPosition(++last_round_robin_index));
        for(SSSlotTypeCurrentQuantumIndexIterator it = set_slots_index_quantum.begin();
            it != set_slots_index_quantum.end();
            it++) {
            //decrement every index
            set_slots_index_quantum.modify(it, decrement_quantum_op);
            
            //chec if we need to execute the slot fetch operation
            if(it->current_quantum < 0) {
                //we need to push it within the execution queue
                queue_active_slot.push(it->quantum_slot.get());
            }
        }
        //awake processing thread
        condition_fetch.notify_one();
        
        //we need to slee for the quantum
        milliseconds_to_sleep = ((current_processing_time = (TimingUtil::getTimeStamp() - milliseconds_start)) - MONITOR_QUANTUM_LENGTH);
        
        //----stat procesisng time---
        current_processing_time += current_processing_time;
        
        if(((stat_counter++) % STAT_ITERATION) == 0) {
            boost::str(boost::format("Stat on processing time is %1% milliseocnds for %2% iteration")%((double)current_processing_time/STAT_ITERATION)%STAT_ITERATION);
            current_processing_time = 0;
        }
        
        if(milliseconds_to_sleep>0) {
            //waith for the time
            boost::this_thread::sleep_for(boost::chrono::milliseconds(0));
        }else{
            QSS_DBG << "Scan thread to slow we need more thread here";
        }
    }
}

void QuantumSlotScheduler::fetchValue() {
    QuantumSlot *cur_slot = NULL;

    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition);
    while(work_on_fetch) {
        if(queue_active_slot.pop(cur_slot)) {
            //we have slot available
        } else {
            //we need to sleep untile someone wakeup us
            condition_fetch.wait(lock_on_condition);
        }
    }
}

//! add a new quantum slot for key
void QuantumSlotScheduler::addKeyConsumer(const std::string& key_to_monitor,
                                          int quantum_multiplier,
                                          QuantumSlotConsumer *consumer,
                                          int consumer_priority) {
    boost::unique_lock<boost::shared_mutex> wl(set_slots_mutex);
    //add the new slot
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    boost::shared_ptr<QuantumSlot> quantum_slot;
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) {
        //we need to create a new  slot
        boost::shared_ptr<QuantumSlot> quantum_slot(new QuantumSlot(key_to_monitor, quantum_multiplier));
        
        //insert new slot
        set_slots.insert(ScheduleSlot(quantum_slot));
    } else {
        // we already have the slot
        quantum_slot = it->quantum_slot;
    }
    //unlock because now we have the shared pointer
    wl.unlock();
    
    //! add consumer to slot
    quantum_slot->addNewConsumer(consumer, consumer_priority);
}

void QuantumSlotScheduler::removeKeyConsumer(const std::string& key_to_monitor,
                                             int quantum_multiplier,
                                             QuantumSlotConsumer *consumer) {
    boost::unique_lock<boost::shared_mutex> wl(set_slots_mutex);
    //add the new slot
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) return;
    // we already have the slot
    boost::shared_ptr<QuantumSlot> quantum_slot = it->quantum_slot;
    //unlock because now we have the shared pointer
    wl.unlock();
    
    //! add consumer to slot
    quantum_slot->removeConsumer(consumer);
}