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
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <iostream>

#include <boost/timer/timer.hpp>

#include "NodeMonitor.h"
#include "HandlerMonitor.h"
#include "NodeSearchTest.h"
#include "NodeMonitorHandlerTest.h"

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::common::utility;
using namespace chaos::common::data;

#define MSCT_INFO   INFO_LOG(MetadataServiceClientTest)
#define MSCT_DBG    INFO_LOG(MetadataServiceClientTest)
#define MSCT_ERR    INFO_LOG(MetadataServiceClientTest)

class AlertLogHandlerImpl:
public chaos::metadata_service_client::event::alert::AlertLogEventHandler {
public:
    void handleLogEvent(const std::string& source,
                        const std::string& domain) {
        MSCT_INFO << source << "-" << domain;
    }
};

#define NUMBER_OF_TEST_ELEMENT 2

boost::thread_group thread_group_test;

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
        
        
        //register log allert event
        ChaosMetadataServiceClient::getInstance()->registerEventHandler(&alert_log_handler);
        switch (operation){
            case 0:{
                
                ChaosMetadataServiceClient::getInstance()->enableMonitor();
                if (device_id.size() == 0) {LOG_AND_TROW(MSCT_ERR, -1, "Invalid device id")}
                if (quantum_multiplier == 0) {LOG_AND_TROW(MSCT_ERR, -2, "Quantum multiplier can't be 0")}
                //create monitor class
                NodeMonitor nm(device_id,
                               wait_seconds,
                               quantum_multiplier);
                
                nm.monitor_node();
                //nm.waitForPurge();
                break;
            }
            case 1:{
                //create search node utility class
                NodeSearchTest ns(5);
                //try search and waith the termination
                ns.testSearch(device_id.size()?device_id:"");
                break;
            }
                
                
            case 2:{
                ChaosMetadataServiceClient::getInstance()->enableMonitor();
                std::cout << "Start node monitor library test" << std::endl;
                {
                    std::auto_ptr<NodeMonitorHandlerTest> nmt;
                    
                    //nmt[0].reset(new NodeMonitorHandlerTest(device_id, chaos::metadata_service_client::node_monitor::ControllerTypeNode));
                    nmt.reset(new NodeMonitorHandlerTest(device_id, chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit));
                    sleep(wait_seconds);
                    //nmt[0].reset();
                    nmt.reset();
                    
                }
                std::cout << "End node monitor library test" << std::endl;
                break;
            }
                
            case 3: {
                bool work = true;
                uint64_t start_ts = chaos::common::utility::TimingUtil::getTimeStamp();
                chaos::metadata_service_client::node_controller::CUController *cu_ctrl = NULL;
                ChaosMetadataServiceClient::getInstance()->getNewCUController(device_id,
                                                                              &cu_ctrl);
                if(cu_ctrl == NULL) throw chaos::CException(-1, CHAOS_FORMAT("No cu controller found for %1%", %device_id), __PRETTY_FUNCTION__);
                
                while(work) {
                    std::cout << "Call init" << std::endl;
                    cu_ctrl->initDevice();
                    sleep(1);
                    std::cout << "Call start" << std::endl;
                    cu_ctrl->startDevice();
                    sleep(1);
                    std::cout << "Call stop" << std::endl;
                    cu_ctrl->stopDevice();
                    sleep(1);
                    std::cout << "Call deinit" << std::endl;
                    cu_ctrl->deinitDevice();
                    sleep(1);
                    work = ((chaos::common::utility::TimingUtil::getTimeStamp()-start_ts) < (wait_seconds*1000));
                }
                
                ChaosMetadataServiceClient::getInstance()->deleteCUController(cu_ctrl);
                break;
            }
                
            case 4: {
                bool work = true;
                CDataWrapper *ds = NULL;
                uint64_t start_ts = TimingUtil::getTimeStamp();
                chaos::metadata_service_client::node_controller::CUController *cu_ctrl = NULL;
                ChaosMetadataServiceClient::getInstance()->getNewCUController(device_id,
                                                                              &cu_ctrl);
                if(cu_ctrl == NULL) throw chaos::CException(-1, CHAOS_FORMAT("No cu controller found for %1%", %device_id), __PRETTY_FUNCTION__);
                uint64_t counter = 0;
                uint64_t start_stat_ts = TimingUtil::getTimeStamp();
                uint64_t last_stat_ts = start_stat_ts;
                while(work) {
                    start_stat_ts = TimingUtil::getTimeStamp();
                    ds = cu_ctrl->fetchCurrentDatatasetFromDomain(chaos::cu::data_manager::KeyDataStorageDomainOutput);
                    if(ds) {
                        counter++;
                    }
                    if((start_stat_ts - last_stat_ts) >= 1000) {
                        std::cout << CHAOS_FORMAT("%1% fetch in %2% milliseconds", %counter%(start_stat_ts - last_stat_ts)) << std::endl;
                        counter = 0;
                        last_stat_ts = start_stat_ts;
                    }
                    work = ((start_stat_ts-start_ts) < (wait_seconds*1000));
                }
                ChaosMetadataServiceClient::getInstance()->deleteCUController(cu_ctrl);
                break;
            }
                
            case 5:{
                bool work = true;
                CDataWrapper *ds = NULL;
                uint64_t start_ts = TimingUtil::getTimeStamp();
                chaos::metadata_service_client::node_controller::CUController *cu_ctrl = NULL;
                while(work){
                    ChaosMetadataServiceClient::getInstance()->getNewCUController(device_id,&cu_ctrl);
                    ds = cu_ctrl->fetchCurrentDatatasetFromDomain(chaos::cu::data_manager::KeyDataStorageDomainOutput);
                    ChaosMetadataServiceClient::getInstance()->deleteCUController(cu_ctrl);
                    work = ((chaos::common::utility::TimingUtil::getTimeStamp()-start_ts) < (wait_seconds*1000));
                }
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
