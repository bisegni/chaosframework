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
ios(),
async_timer(ios, boost::posix_time::seconds(0)),
async_work(ios),
network_broker(_network_broker),
data_driver(NULL),
queue_active_slot(100),
queue_new_quantum_slot_consumer(100),
set_slots_index_quantum(boost::multi_index::get<ss_current_quantum>(set_slots)),
set_slots_index_key_slot(boost::multi_index::get<ss_quantum_slot_key>(set_slots)) {
    
}

QuantumSlotScheduler::~QuantumSlotScheduler() {
    
}


void QuantumSlotScheduler::init(void *init_data) throw (chaos::CException) {
    CHAOS_LASSERT_EXCEPTION(network_broker, QSS_ERR, -1, "No network broker instance found")
    data_driver_impl = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL);
    CHAOS_LASSERT_EXCEPTION((data_driver_impl.compare("IODirect") == 0), QSS_ERR, -2, "Only IODirect implementation is supported")
}

void QuantumSlotScheduler::start() throw (chaos::CException) {
    work_on_scan = true;
    work_on_fetch = true;
    
    //launch asio thread
    ios_thread.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &ios)));
    
    //add scanner thread
    scanner_threads.add_thread(new boost::thread(bind(&QuantumSlotScheduler::scanSlot, this)));
    
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
    scanner_threads.join_all();
    QSS_INFO<< "Scanner thread stopped";
    
    QSS_INFO<< "Stop fetcher thread";
    condition_fetch.notify_all();
    fetcher_threads.join_all();
    QSS_INFO<< "Fetcher thread stopped";
    
    //stop asio thread
    QSS_INFO<< "Stop forwarded ASIO infrastructure";
    ios.stop();
    QSS_INFO<< "ASIO Stopped";
}

void QuantumSlotScheduler::deinit() throw (chaos::CException) {
    QSS_INFO<< "Clean unmanaged slot consumer add and remove request";
    SlotConsumerInfo *ci = NULL;
    while(queue_new_quantum_slot_consumer.pop(ci)){
        if(!ci) continue;
        try {
            if(ci->operation) {
                //we need to add it
                QSS_INFO << boost::str(boost::format("The key consumer [%1%-%2%-%3%] can't be added we are stopping all")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);
            } else {
                _removeKeyConsumer(ci);
            }
        } catch(...){
            
        }
        delete(ci);
    }
}

//! set a new lits of server to use for fetch values
void QuantumSlotScheduler::setDataDriverEndpoints(const std::vector<std::string>& _data_driver_endpoint) {
    QSS_INFO<< "Update the server list";
    
    data_driver_endpoint = _data_driver_endpoint;
    QSS_INFO<< "New servers has been configured";
    if(fetcher_threads.size() == 0) return; //no thread are active so never has been started
    
    work_on_fetch = false;
    condition_scan.notify_all();
    fetcher_threads.join_all();
    QSS_INFO<< "Stoppped all current fetcher thread";
    
    work_on_fetch = true;
    condition_fetch.notify_all();
    addNewfetcherThread();
    QSS_INFO<< "New fetcher thread created";
}

void QuantumSlotScheduler::scanSlot() {
    int         stat_counter = 0;
    int64_t     milliseconds_to_sleep = 0;
    uint64_t    milliseconds_start = 0;
    uint64_t    current_processing_time = 0;
    uint64_t    iteration_processing_time = 0;
    int         processed_element;
    ScheduleSlotDecrementQuantum        decrement_quantum_op;
    ScheduleSlotDisableQuantum          disable_quantum_op;
    
    QSS_INFO << "Enter scan thread:" << boost::this_thread::get_id();
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    while(work_on_scan) {
        processed_element = 0;
        milliseconds_start = TimingUtil::getTimeStamp();
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
        
        //we need to slee for the quantum
        milliseconds_to_sleep = (MONITOR_QUANTUM_LENGTH - (current_processing_time = (TimingUtil::getTimeStamp() - milliseconds_start)));
        
        //----stat procesisng time---
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
            //whait for the time
            do {
                condition_scan.timed_wait(lock_on_condition, posix_time::milliseconds(milliseconds_to_sleep));
                
                //!check for new consumers
                _checkRemoveAndAddNewConsumer();
            }while((set_slots_index_quantum.size()==0) && work_on_scan);
        }else{
            QSS_INFO << "Scan thread to slow we need more thread here";
        }
    }
    //clean the queue and process only delete operation
    QSS_INFO << "Leaving scan thread:" << boost::this_thread::get_id();
}

bool QuantumSlotScheduler::_checkRemoveAndAddNewConsumer() {
    bool something_has_been_processed = true;
    SlotConsumerInfo *new_consumer_info = NULL;
    //! try to get new one
    if(!(something_has_been_processed = queue_new_quantum_slot_consumer.pop(new_consumer_info))) return something_has_been_processed;
    try {
        if(new_consumer_info->operation) {
            //we need to add it
            _addKeyConsumer(new_consumer_info);
        } else {
            _removeKeyConsumer(new_consumer_info);
        }  
    } catch(...){
        
    }
    delete(new_consumer_info);
    return something_has_been_processed;
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
    //get client and endpoint
    init_param.network_broker = network_broker;
    init_param.client_instance = NULL;
    init_param.endpoint_instance = NULL;
    ((IODirectIODriver*)data_driver)->setDirectIOParam(init_param);
    
    data_driver->init(NULL);
    
    for(std::vector<std::string>::iterator it = data_driver_endpoint.begin();
        it != data_driver_endpoint.end();
        it++) {
        ((IODirectIODriver*)data_driver)->addServerURL(*it);
    }
    
    fetcher_threads.add_thread(new boost::thread(bind(&QuantumSlotScheduler::fetchValue, this, boost::shared_ptr<IODataDriver>(data_driver))));
}

void QuantumSlotScheduler::dispath_new_value_async(const boost::system::error_code& error,
                                                   QuantumSlot *cur_slot,
                                                   const char *data_found) {
    if(data_found) {
        cur_slot->sendNewValueConsumer(KeyValue(new CDataWrapper(data_found)));
        delete(data_found);
    } else {
        cur_slot->sendNoValueToConsumer();
    }
    //reset quantum
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(cur_slot->key, cur_slot->quantum_multiplier);
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    set_slots_index_key_slot.modify(it, ScheduleSlotResetQuantum());
}

void QuantumSlotScheduler::fetchValue(boost::shared_ptr<IODataDriver> data_driver) {
    QuantumSlot *cur_slot = NULL;
    QSS_INFO << "Entering fetcher thread:" << boost::this_thread::get_id();
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_fetch);
    while(work_on_fetch) {
        if(queue_active_slot.pop(cur_slot)) {
            //we have slot available
            size_t          data_found_size;
            const char * data_found = data_driver->retriveRawData(cur_slot->key, &data_found_size);
            async_timer.async_wait(boost::bind(&QuantumSlotScheduler::dispath_new_value_async,
                                               this,
                                               boost::asio::placeholders::error,
                                               cur_slot,
                                               data_found));
        } else {
            //we need to sleep untile someone wakeup us
            QSS_DBG << "No more data slot to fetch so i'm going to sleep";
            condition_fetch.wait(lock_on_condition);
        }
    }
    data_driver->deinit();
    QSS_INFO << "Leaving fetcher thread:" << boost::this_thread::get_id();
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
    if(map_quantum_slot_consumer.count(quantum_consumer_key)) {
        QSS_ERR << boost::str(boost::format("Key consumer alredy registere [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
        return;
    }
    QSS_INFO << boost::str(boost::format("Add new key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);

    //increment the index to indicate that has passed scheduler publi layer
    consumer->free_of_work = false;
    consumer->usage_counter++;
    
    //push into lock free queue for add the consumer
    queue_new_quantum_slot_consumer.push(new SlotConsumerInfo(true,
                                                              key_to_monitor,
                                                              quantum_multiplier,
                                                              consumer,
                                                              consumer_priority));
}


void QuantumSlotScheduler::removeKeyConsumer(const std::string& key_to_monitor,
                                             unsigned int quantum_multiplier,
                                             QuantumSlotConsumer *consumer) {
    CHAOS_ASSERT(consumer)
    QSS_INFO << boost::str(boost::format("Start removing key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);

    //prepare key
    std::string quantum_consumer_key = CHAOS_QSS_COMPOSE_QUANTUM_CONSUMER_KEY(key_to_monitor,
                                                                              quantum_multiplier,
                                                                              consumer);
    
    //decrement the index to indicate that it has been remove from public layer
    consumer->usage_counter--;
    
    //push into lock free queue to remove the consumer
    queue_new_quantum_slot_consumer.push(new SlotConsumerInfo(false,
                                                              key_to_monitor,
                                                              quantum_multiplier,
                                                              consumer,
                                                              0));
    
    //waith for completiotion on consumer
    consumer->waitForCompletition();
    QSS_INFO << boost::str(boost::format("Cleanly removed key consumer [%1%-%2%-%3%]")%key_to_monitor%quantum_multiplier%consumer);
}

//---------called by the scheduler thread----------
void QuantumSlotScheduler::_addKeyConsumer(SlotConsumerInfo *ci) {
    CHAOS_ASSERT(ci)
    boost::shared_ptr<QuantumSlot> quantum_slot;
    
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(ci->key_to_monitor, ci->quantum_multiplier);
    
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) {
        //we need to create a new  slot
        quantum_slot.reset(new QuantumSlot(ci->key_to_monitor, ci->quantum_multiplier));
        
        //insert new slot
        set_slots.insert(ScheduleSlot(quantum_slot));
    } else {
        // we already have the slot
        quantum_slot = it->quantum_slot;
    }
    
    //! add consumer to slot
    quantum_slot->addNewConsumer(ci->consumer, ci->consumer_priority);
    DEBUG_CODE(QSS_DBG << boost::str(boost::format("Added key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
}

void QuantumSlotScheduler::_removeKeyConsumer(SlotConsumerInfo *ci) {
    CHAOS_ASSERT(ci)
    DEBUG_CODE(QSS_DBG << boost::str(boost::format("Start removing key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)

    //prepare key
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(ci->key_to_monitor,
                                                                      ci->quantum_multiplier);

    
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) return;
    // we already have the slot
    boost::shared_ptr<QuantumSlot> quantum_slot = it->quantum_slot;
    //try to remove the consumer, if we dont have success, it mean that the consumer has been removed directly from quantum sot object
    if(quantum_slot->removeConsumer(ci->consumer)) {
        DEBUG_CODE(QSS_DBG << boost::str(boost::format("we have removed key consumer [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
        //we have succesfull remove it and we can notify
        ci->consumer->setFreeOfWork();
        DEBUG_CODE(QSS_DBG << boost::str(boost::format("we have signaled key consumer that it is free of work [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    } else {
        DEBUG_CODE(QSS_DBG << boost::str(boost::format("The key consumer has been removed directly by the slot [%1%-%2%-%3%]")%ci->key_to_monitor%ci->quantum_multiplier%ci->consumer);)
    }
}