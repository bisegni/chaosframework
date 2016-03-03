/*
 *	main.cpp
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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <iostream>

#include <boost/timer/timer.hpp>

#include "NodeMonitor.h"
#include "HandlerMonitor.h"
#include "NodeSearchTest.h"

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;
using namespace chaos::metadata_service_client::api_proxy;

#define MSCT_INFO   INFO_LOG(MetadataServiceClientTest)
#define MSCT_DBG    INFO_LOG(MetadataServiceClientTest)
#define MSCT_ERR    INFO_LOG(MetadataServiceClientTest)

class AlertLogHandlerImpl:
public chaos::metadata_service_client::event::alert::AlertLogEventHandler {
public:
    void handleLogEvent(const std::string source,
                        const std::string domain) {
        MSCT_INFO << source << "-" << domain;
    }
};

#define NUMBER_OF_TEST_ELEMENT 10

boost::thread_group thread_group_test;

void executeHandlerTest(){
    HandlerMonitor hm("BTF/DHSTB001_healt",
                      "nh_status");
    hm.init();
    usleep(2000000);
    hm.deinit();
}

int main(int argc, char *argv[]){
    boost::thread_group tg;
    uint32_t quantum_multiplier;
    uint32_t wait_seconds;
    uint32_t operation;
    std::string device_id;
    AlertLogHandlerImpl alert_log_handler;
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("op",
                                                                                                     "Specify the operation to do[0-monitor a device id, 1-search node id]",
                                                                                                     &operation);
    
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>("device-id",
                                                                                                        "Specify the device",
                                                                                                        &device_id);
    
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("qm",
                                                                                                     "Specify the quantum multiplier to use",
                                                                                                     1,
                                                                                                     &quantum_multiplier);
    
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("monitor-timeout",
                                                                                                     "Specify the time that we need to monitor the device in seconds",
                                                                                                     10,
                                                                                                     & wait_seconds);
    try{
        
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        
        ChaosMetadataServiceClient::getInstance()->start();
        
        ChaosMetadataServiceClient::getInstance()->enableMonitor();
        
        //register log allert event
        ChaosMetadataServiceClient::getInstance()->registerEventHandler(&alert_log_handler);
        
        switch (operation){
            case 0:{
                
                if (device_id.size() == 0) {LOG_AND_TROW(MSCT_ERR, -1, "Invalid device id")}
                if (quantum_multiplier == 0) {LOG_AND_TROW(MSCT_ERR, -2, "Quantum multiplier can't be 0")}
                //create monitor class
                NodeMonitor nm(device_id,
                               wait_seconds,
                               quantum_multiplier);
                
                boost::shared_ptr<NodeMonitor> arr[NUMBER_OF_TEST_ELEMENT];
                boost::shared_ptr<HandlerMonitor> arr_handler[NUMBER_OF_TEST_ELEMENT];
                
                
                for(int idx= 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    arr[idx].reset(new NodeMonitor(device_id,
                                                   wait_seconds,
                                                   quantum_multiplier));
                    arr[idx]->registerConsumer();
                }
                
                for(int idx= 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    arr_handler[idx].reset(new HandlerMonitor("BTF/DHSTB001_healt",
                                                              "nh_status"));
                    arr_handler[idx]->init();
                }
                
                sleep(5);
                for(int idx= 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    if(arr[idx]->deregisterConsumer()){
                        arr[idx].reset();
                    }
                }
                
                for(int idx= 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    arr_handler[idx]->deinit();
                    arr_handler[idx].reset();
                }
                
                for(int idx= 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    if(arr[idx].get())arr[idx]->waitForPurge();
                }
                
                //nm.waitForPurge();
                break;
            }
            case 1:{
                //create search node utility class
                NodeSearchTest ns(5);
                //try search and waith the termination
                ns.testSearch(device_id.size()?device_id:"");
            }
                
                
            case 2:{
                for(int idx = 0; idx < NUMBER_OF_TEST_ELEMENT; idx++) {
                    thread_group_test.add_thread(new boost::thread(boost:: bind(executeHandlerTest)));
                }
                sleep(2);
                thread_group_test.join_all();
            }
                
        }
        
        //register log allert event
        ChaosMetadataServiceClient::getInstance()->deregisterEventHandler(&alert_log_handler);
        
        //ChaosMetadataServiceClient::getInstance()->disableMonitor();
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    }
    catch (chaos::CException &ex){
        DECODE_CHAOS_EXCEPTION(ex)
    }
    catch (...){
        std::cerr << "Unrecognized error";
    }
    return 0;
}
