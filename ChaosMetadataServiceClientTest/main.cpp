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

#include "NodeMonitor.h"
#include "NodeSearchTest.h"

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;
using namespace chaos::metadata_service_client::api_proxy::node;

#define MSCT_INFO   INFO_LOG(MetadataServiceClientTest)
#define MSCT_DBG    INFO_LOG(MetadataServiceClientTest)
#define MSCT_ERR    INFO_LOG(MetadataServiceClientTest)

int main(int argc, char *argv[]){
    boost::thread_group tg;
    uint32_t quantum_multiplier;
    uint32_t wait_seconds;
    uint32_t operation;
    std::string device_id;
    
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
        
        switch (operation){
            case 0:{
                
                if (device_id.size() == 0) {LOG_AND_TROW(MSCT_ERR, -1, "Invalid device id")}
                if (quantum_multiplier == 0) {LOG_AND_TROW(MSCT_ERR, -2, "Quantum multiplier can't be 0")}
                //create monitor class
                NodeMonitor nm(device_id,
                               wait_seconds,
                               quantum_multiplier);
                //start and wait for monitor termination
                nm.monitor_node();
                break;
            }
            case 1:{
                //if(!ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->hasOption("device-id")){LOG_AND_TROW(MSCT_ERR, -3, "Device id is usede in search, as search string")}
                //create search node utility class
                NodeSearchTest ns(5);
                
                //try search and waith the termination
                ns.testSearch(device_id.size()?device_id:"");
            }
        }
        
        ChaosMetadataServiceClient::getInstance()->disableMonitor();
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
