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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

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
        MSCT_INFO << "EVENT---->" << source << "-" << domain;
    }
};


int main(int argc, char *argv[]){
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
    //register log allert event
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);
    ChaosMetadataServiceClient::getInstance()->start();
    ChaosMetadataServiceClient::getInstance()->enableMonitor();
    try{
        switch (operation){
            case 0:{
                
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
        }
        
    }
    catch (chaos::CException &ex){
        DECODE_CHAOS_EXCEPTION(ex)
    }
    catch (...){
        std::cerr << "Unrecognized error";
    }
    ChaosMetadataServiceClient::getInstance()->disableMonitor();
    ChaosMetadataServiceClient::getInstance()->stop();
    ChaosMetadataServiceClient::getInstance()->deinit();
    return 0;
}
