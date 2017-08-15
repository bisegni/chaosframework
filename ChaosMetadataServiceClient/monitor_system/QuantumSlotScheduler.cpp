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

#include <ChaosMetadataServiceClient/monitor_system/QuantumSlotScheduler.h>

#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>

#include <boost/format.hpp>

#define STAT_ITERATION 100

#define QSS_INFO   INFO_LOG (QuantumSlotScheduler)
#define QSS_DBG    DBG_LOG  (QuantumSlotScheduler)
#define QSS_ERR    ERR_LOG  (QuantumSlotScheduler)

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::metadata_service_client::monitor_system;

QuantumSlotScheduler::QuantumSlotScheduler(chaos::common::network::NetworkBroker *_network_broker):
network_broker(_network_broker),
queue_active_slot(100),
set_slots_index_quantum(boost::multi_index::get<ss_current_quantum>(set_slots)),
set_slots_index_key_slot(boost::multi_index::get<ss_quantum_slot_key>(set_slots)) {}

QuantumSlotScheduler::~QuantumSlotScheduler() {}


void QuantumSlotScheduler::init(void *init_data) throw (chaos::CException) {
    CHAOS_LASSERT_EXCEPTION(network_broker, QSS_ERR, -1, "No network broker instance found")
    data_driver_impl = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL);
    CHAOS_LASSERT_EXCEPTION((data_driver_impl.compare("IODirect") == 0), QSS_ERR, -2, "Only IODirect implementation is supported")
}

void QuantumSlotScheduler::start() throw (chaos::CException) {
    work_on_scan = true;
    work_on_fetch = true;
    
    //add scanner thread
    scanner_threads.reset(new boost::thread_group());
    fetcher_threads.reset(new boost::thread_group());
    scanner_threads->add_thread(new boost::thread(bind(&QuantumSlotScheduler::scanSlot, this)));
    
    //add first fetcher thread
    if(data_driver_endpoint.size()) {
        addNewfetcherThread();
    }
}

void QuantumSlotScheduler::stop() throw (chaos::CException) {
    work_on_scan = false;
    work_on_fetch = false;
    
    QSS_INFO<< "Stop scanner thread";
    condition_scan.notify_all();
    scanner_threads->join_all();
    scanner_threads.reset();
    QSS_INFO<< "Scanner thread stopped";
    
    QSS_INFO<< "Stop fetcher thread";
    condition_fetch.notify_all();
    fetcher_threads->join_all();
    fetcher_threads.reset();
    QSS_INFO<< "Fetcher thread stopped";
}

void QuantumSlotScheduler::deinit() throw (chaos::CException) {
    
    QSS_INFO<< "Clean unmanaged slot consumer add and remove request";
    SlotConsumerInfo *ci = NULL;
    LOCK_QUEUE(queue_new_quantum_slot_consumer);
    
    while(queue_new_quantum_slot_consumer.size()){
        ci = queue_new_quantum_slot_consumer.front(); queue_new_quantum_slot_consumer.pop();
        ChaosUniquePtr<SlotConsumerInfo> auto_ci(ci);
        try {
            if(ci->operation) {
                //we need to add it
                QSS_INFO << boost::str(boost::format("The key consumer [%1%-%2%-%3%] can't be added we are stopping all")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);
            } else {
                _removeKeyConsumer(auto_ci.get());
            }
        } catch(...){
            
        }
        //delete(ci);
    }
}

//! set a new lits of server to use for fetch values
void QuantumSlotScheduler::setDataDriverEndpoints(const std::vector<std::string>& _data_driver_endpoint) {
    QSS_INFO<< "Update the server list";
    
    data_driver_endpoint = _data_driver_endpoint;
    QSS_INFO<< "New servers has been configured";
    if(fetcher_threads.get() == NULL ||
       fetcher_threads->size() == 0) return; //no thread are active so never has been started
    
    stop();
    start();
}

void QuantumSlotScheduler::scanSlot() {
    int         stat_counter = 0;
    int64_t     milliseconds_to_sleep = 0;
    uint64_t    milliseconds_start = 0;
    uint64_t    current_processing_time = 0;
    uint64_t    iteration_processing_time = 0;
    uint64_t    start_purge_time = 0;
    int         processed_element;
    ScheduleSlotDecrementQuantum        decrement_quantum_op;
    ScheduleSlotDisableQuantum          disable_quantum_op;
    
    QSS_INFO << "Enter scan thread";
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan, boost::defer_lock);
    while(work_on_scan) {
        processed_element = 0;
        milliseconds_start = TimingUtil::getTimeStamp();
        lock_on_condition.lock();
        for(SSSlotTypeCurrentQuantumIndexIterator it = set_slots_index_quantum.begin();
            it != set_slots_index_quantum.end();
            it++) {
            processed_element++;
            //decrement every index
            set_slots_index_quantum.modify(it, decrement_quantum_op);
            
            //chec if we need to execute the slot fetch operation
            if(it->current_quantum == 0) {
                //tag slot as in processing mode
                set_slots_index_quantum.modify(it, disable_quantum_op);
                
                //we need to push it within the execution queue
                queue_active_slot.push(it->quantum_slot.get());
                
                //awake processing thread
                condition_fetch.notify_one();
            }
        }
        lock_on_condition.unlock();
        
        //we need to slee for the quantum
        milliseconds_to_sleep = (MONITOR_QUANTUM_LENGTH - (current_processing_time = ((start_purge_time = TimingUtil::getTimeStamp()) - milliseconds_start)));
        
        //----stat processing time---
        iteration_processing_time += current_processing_time;
        
        if(((stat_counter++) % STAT_ITERATION) == 0) {
            QSS_INFO << boost::str(boost::format("Slot Processing time is ~%1% milliseconds(total milliseconds %2% for %3% quantum) for %4% elements")%
                                   ((double)iteration_processing_time/STAT_ITERATION)%
                                   iteration_processing_time%
                                   STAT_ITERATION%
                                   processed_element);
            iteration_processing_time = 0;
        }
        
        if(milliseconds_to_sleep>0) {
            //at this point we need to check if we need to do some work before sleep
            //!check for new consumers
            
            milliseconds_to_sleep = _checkRemoveAndAddNewConsumer(start_purge_time,
                                                                  milliseconds_to_sleep);
            //whait for the time if we still have some
            if(milliseconds_to_sleep>0){
                //condition_scan.timed_wait(lock_on_condition, posix_time::milliseconds(milliseconds_to_sleep));
                boost::this_thread::sleep_for(boost::chrono::milliseconds(milliseconds_to_sleep));
            }
        }else{
            QSS_INFO << "Scan thread to slow we need more thread here";
        }
    }
    //clean the queue and process only delete operation
    QSS_INFO << "Leaving scan thread";
}

uint64_t QuantumSlotScheduler::_checkRemoveAndAddNewConsumer(uint64_t start_time_in_milliseconds,
                                                             uint64_t millisecond_for_work) {
    if(millisecond_for_work == 0) return millisecond_for_work;
    SlotConsumerInfo *new_consumer_info = NULL;
    bool has_worked = false;
    bool can_process_other = true;
    uint64_t start_processing_time = start_time_in_milliseconds;
    uint64_t end_processing_time = start_time_in_milliseconds;
    int64_t max_processing_time = millisecond_for_work;
    int64_t single_processing_time = 0;
    
    //scan element for the maximum milliseocnds we can
    LOCK_QUEUE(queue_new_quantum_slot_consumer);
    while(queue_new_quantum_slot_consumer.size() &&
          can_process_other){
        new_consumer_info = queue_new_quantum_slot_consumer.front(); queue_new_quantum_slot_consumer.pop();
        has_worked = true;
        try {
            ChaosUniquePtr<SlotConsumerInfo> auto_ci(new_consumer_info);
            if(auto_ci->operation) {
                //we need to add it
                DEBUG_CODE(QSS_INFO << boost::str(boost::format("Asynchronously add key consumer [%1%-%2%-%3%]")%auto_ci->key_to_monitor%auto_ci->quantum_multiplier%auto_ci->consumer);)
                _addKeyConsumer(auto_ci.get());
            } else {
                DEBUG_CODE(QSS_INFO << boost::str(boost::format("Asynchronously remove key consumer [%1%-%2%-%3%]")%auto_ci->key_to_monitor%auto_ci->quantum_multiplier%auto_ci->consumer);)
                DEBUG_CODE(QSS_DBG<< "Asynchronously removed consumer [" << new_consumer_info->key_to_monitor <<"-" << new_consumer_info->quantum_multiplier << "]";)
                _removeKeyConsumer(auto_ci.get());
            }
        } catch(...){
            
        }
        
        single_processing_time = ((end_processing_time = TimingUtil::getTimeStamp()) - start_processing_time);
        can_process_other  = ((max_processing_time -= single_processing_time) > single_processing_time);
        DEBUG_CODE(QSS_DBG << "max_processing_time:" << max_processing_time << " - single_processing_time:" << single_processing_time;)
    }
    if(has_worked){DEBUG_CODE(QSS_DBG << "Leave _checkRemoveAndAddNewConsumer [" << max_processing_time << "]";)}
    //delete(new_consumer_info);
    return (max_processing_time>0)?max_processing_time:0;
}

//! add a new thread to the fetcher job
void QuantumSlotScheduler::addNewfetcherThread() {
    QSS_INFO<< "start the allocation of a new fetcher Thread";
    IODataDriver *data_driver = NULL;
    data_driver = ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(boost::str(boost::format("%1%IODriver")%data_driver_impl));
    CHAOS_LASSERT_EXCEPTION(data_driver, QSS_ERR, -1, "Error allocating new data driver")
    
    //set the information
    IODirectIODriverInitParam init_param;
    std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
    init_param.client_instance = NULL;
    init_param.endpoint_instance = NULL;
    ((IODirectIODriver*)data_driver)->setDirectIOParam(init_param);
    
    data_driver->init(NULL);
    
    for(std::vector<std::string>::iterator it = data_driver_endpoint.begin();
        it != data_driver_endpoint.end();
        it++) {
        ((IODirectIODriver*)data_driver)->addServerURL(*it);
    }
    
    fetcher_threads->add_thread(new boost::thread(bind(&QuantumSlotScheduler::fetchValue, this, ChaosSharedPtr<IODataDriver>(data_driver))));
}

void QuantumSlotScheduler::dispath_new_value_async(const boost::system::error_code& error,
                                                   QuantumSlot *cur_slot,
                                                   char *data_found) {
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(cur_slot->key, cur_slot->quantum_multiplier);
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);

    //check if the slot is empty
    if(cur_slot->size()) {
        //slot has handler so we need to broadcast data to it in this case we unlock to permit other handler to be inserted
        lock_on_condition.unlock();
        try{
            if(data_found) {
                cur_slot->sendNewValueConsumer(KeyValue(new CDataWrapper(data_found)));
            } else {
                cur_slot->sendNoValueToConsumer();
            }
        }catch(...) {
            QSS_ERR << "Exception during data forwarding";
        }
        
        //at this point we need to lock to reset the quantum of the slot
        lock_on_condition.lock();
        
        //find the slot
        SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
        
        //reset it
        set_slots_index_key_slot.modify(it, ScheduleSlotResetQuantum());
    }else{
        QSS_INFO << "Slot for key:" << quantum_slot_key << " has no more handler so we can delete it";
        //we can delete the slot and already own the lock
        SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
        
        //delete the slot
        set_slots_index_key_slot.erase(it);
    }
    if(data_found) {free(data_found);}
}

void QuantumSlotScheduler::fetchValue(ChaosSharedPtr<IODataDriver> data_driver) {
    QuantumSlot *cur_slot = NULL;
    boost::system::error_code error;
    QSS_INFO << "Entering fetcher thread";
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_fetch);
    while(work_on_fetch) {
        if(queue_active_slot.pop(cur_slot)) {
            //we have slot available
            size_t data_found_size;
            char * data_found = data_driver->retriveRawData(cur_slot->key, &data_found_size);
            
            //dispatch data
            dispath_new_value_async(error,
                                    cur_slot,
                                    data_found);
        } else {
            //we need to sleep untile someone wakeup us
            QSS_DBG << "No more data slot to fetch so i'm going to sleep";
            condition_fetch.wait(lock_on_condition);
        }
    }
    data_driver->deinit();
    data_driver.reset();
    QSS_INFO << "Leaving fetcher thread";
}

//! add a new quantum slot for key
void QuantumSlotScheduler::addKeyConsumer(const std::string& key_to_monitor,
                                          unsigned int quantum_multiplier,
                                          QuantumSlotConsumer *consumer,
                                          unsigned int consumer_priority) {
    CHAOS_ASSERT(consumer)
    //add the new slot
    std::string quantum_consumer_key = CHAOS_QSS_COMPOSE_QUANTUM_CONSUMER_KEY(key_to_monitor,
                                                                              quantum_multiplier,
                                                                              consumer);
    //increment the index to indicate that has passed scheduler public layer
    consumer->usage_counter++;
    
    LOCK_QUEUE(queue_new_quantum_slot_consumer);
    //push into lock free queue for add the consumer
    queue_new_quantum_slot_consumer.push(new SlotConsumerInfo(true,
                                                              key_to_monitor,
                                                              quantum_multiplier,
                                                              consumer,
                                                              consumer_priority));
    QSS_INFO << boost::str(boost::format("Submitted creation for new key consumer [%1%-%2%-%3%(%4%)]")%key_to_monitor%quantum_multiplier%consumer%consumer->usage_counter);
}


bool QuantumSlotScheduler::removeKeyConsumer(const std::string& key_to_monitor,
                                             unsigned int quantum_multiplier,
                                             QuantumSlotConsumer *consumer,
                                             bool wait_completion) {
    CHAOS_ASSERT(consumer)
    bool result = true;
    //prepare key
    std::string quantum_consumer_key = CHAOS_QSS_COMPOSE_QUANTUM_CONSUMER_KEY(key_to_monitor,
                                                                              quantum_multiplier,
                                                                              consumer);
    
    //decrement the index to indicate that it has been remove from public layer
    consumer->usage_counter--;
    
    ChaosUniquePtr<SlotConsumerInfo> remove_command(new SlotConsumerInfo(false,
                                                                        key_to_monitor,
                                                                        quantum_multiplier,
                                                                        consumer,
                                                                        0));
    //push into lock free queue to remove the consumer
    if(wait_completion == false) {
        QSS_INFO << boost::str(boost::format("Try to remove directly the key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
        if((result = _removeKeyConsumer(remove_command.get()))== false) {
            QSS_INFO << boost::str(boost::format("Consumer is in use so we submit the remove operation for it [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
            //we can have success to remove so demand the operation asynchronously
            LOCK_QUEUE(queue_new_quantum_slot_consumer);
            queue_new_quantum_slot_consumer.push(remove_command.release());
            UNLOCK_QUEUE(queue_new_quantum_slot_consumer);
        }
    } else {
        QSS_INFO << boost::str(boost::format("Submited remove operation for key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
        LOCK_QUEUE(queue_new_quantum_slot_consumer);
        queue_new_quantum_slot_consumer.push(remove_command.release());
        UNLOCK_QUEUE(queue_new_quantum_slot_consumer);
        //now waith for termination
        QSS_INFO << boost::str(boost::format("Wait for remove of key consumer [%1%-%2%-%3%(%4%)]")%key_to_monitor%quantum_multiplier%consumer%consumer->usage_counter);
        consumer->waitForCompletion();
        QSS_INFO << boost::str(boost::format("Cleanly removed key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
        
    }
    return result;
}

//---------called by the scheduler thread----------
void QuantumSlotScheduler::_addKeyConsumer(SlotConsumerInfo *ci) {
    CHAOS_ASSERT(ci)
    //lock the set
    DEBUG_CODE(QSS_INFO << boost::str(boost::format("Start adding key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    
    ChaosSharedPtr<QuantumSlot> quantum_slot;
    
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(ci->key_to_monitor, ci->quantum_multiplier);
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) {
        DEBUG_CODE(QSS_INFO << boost::str(boost::format("We need to create a new quantum slot for key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
        //we need to create a new  slot
        quantum_slot.reset(new QuantumSlot(ci->key_to_monitor, ci->quantum_multiplier));
        
        //insert new slot
        set_slots.insert(ScheduleSlot(quantum_slot));
    } else {
        DEBUG_CODE(QSS_INFO << boost::str(boost::format("We already have the slot for key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
        // we already have the slot
        quantum_slot = it->quantum_slot;
    }
    
    //! add consumer to slot
    quantum_slot->addNewConsumer(ci->consumer, ci->consumer_priority);
    DEBUG_CODE(QSS_INFO << boost::str(boost::format("Added key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
}

bool QuantumSlotScheduler::_removeKeyConsumer(SlotConsumerInfo *ci) {
    CHAOS_ASSERT(ci)
    //lock the set
    DEBUG_CODE(QSS_INFO << boost::str(boost::format("Start removing key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    
    //prepare key
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(ci->key_to_monitor,
                                                                      ci->quantum_multiplier);
    
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) return true;
    // we already have the slot
    ChaosSharedPtr<QuantumSlot> quantum_slot = it->quantum_slot;
    //try to remove the consumer, if we dont have success, it mean that the consumer has been removed directly from quantum sot object
    if(quantum_slot->removeConsumer(ci->consumer)) {
        DEBUG_CODE(QSS_INFO << boost::str(boost::format("we have removed key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
        //we have succesfull remove it and we can notify
        ci->consumer->setFreeOfWork();
        DEBUG_CODE(QSS_INFO << boost::str(boost::format("we have signaled key consumer that it is free of work [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    } else {
        DEBUG_CODE(QSS_INFO << boost::str(boost::format("The key consumer has not been found in the slot [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    }
    return true;
}
