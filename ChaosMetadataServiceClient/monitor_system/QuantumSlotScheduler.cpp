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
        //set_urls_rb_pos_index.modify(it, URLServiceIndexUpdateRBPosition(++last_round_robin_index));
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
            }while((set_slots_index_quantum.size()==0) && work_on_scan);
        }else{
            QSS_INFO << "Scan thread to slow we need more thread here";
        }
    }
    QSS_INFO << "Leaving scan thread:" << boost::this_thread::get_id();
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
                                          int quantum_multiplier,
                                          QuantumSlotConsumer *consumer,
                                          unsigned int consumer_priority) {
    //boost::unique_lock<boost::shared_mutex> wl(set_slots_mutex);
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    
    //add the new slot
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    boost::shared_ptr<QuantumSlot> quantum_slot;
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) {
        //we need to create a new  slot
        quantum_slot.reset(new QuantumSlot(key_to_monitor, quantum_multiplier));
        
        //insert new slot
        set_slots.insert(ScheduleSlot(quantum_slot));
    } else {
        // we already have the slot
        quantum_slot = it->quantum_slot;
    }
    
    //! add consumer to slot
    quantum_slot->addNewConsumer(consumer, consumer_priority);
}

void QuantumSlotScheduler::removeKeyConsumer(const std::string& key_to_monitor,
                                             int quantum_multiplier,
                                             QuantumSlotConsumer *consumer) {
    //boost::unique_lock<boost::shared_mutex> wl(set_slots_mutex);
    boost::unique_lock<boost::mutex> lock_on_condition(mutex_condition_scan);
    //add the new slot
    std::string quantum_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    
    SSSlotTypeQuantumSlotKeyIndexIterator it = set_slots_index_key_slot.find(quantum_slot_key);
    if(it == set_slots_index_key_slot.end()) return;
    // we already have the slot
    boost::shared_ptr<QuantumSlot> quantum_slot = it->quantum_slot;
    
    //! add consumer to slot
    quantum_slot->removeConsumer(consumer);
    
   // if(quantum_slot->size() == 0) {
     //   set_slots_index_key_slot.erase(it);
   // }
}