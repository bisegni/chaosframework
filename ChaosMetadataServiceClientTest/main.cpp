//
//  main.cpp
//  ChaosMetadataServiceClientTest
//
//  Created by Claudio Bisegni on 27/03/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <iostream>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;
using namespace chaos::metadata_service_client::api_proxy::node;

boost::atomic<uint64_t> global_counter;
boost::atomic<uint64_t> error_count;
boost::atomic<uint64_t> no_result_count;

#define MSCT_INFO   INFO_LOG(MetadataServiceClientTest)
#define MSCT_DBG    INFO_LOG(MetadataServiceClientTest)
#define MSCT_ERR    INFO_LOG(MetadataServiceClientTest)

class EchoTestProxy:
public chaos::metadata_service_client::api_proxy::ApiProxy {
    API_PROXY_CLASS(EchoTestProxy)
protected:
    //! default constructor
    EchoTestProxy(chaos::common::message::MultiAddressMessageChannel *_mn_message,
                  int32_t timeout_in_milliseconds):
    ApiProxy("test", "echo", _mn_message, timeout_in_milliseconds){};
    //! default destructor
    ~EchoTestProxy(){};
public:
    
    /*!
     Return the description of the node
     */
    ApiProxyResult execute(const std::string& echo_test_key,
                           const std::string& echo_test_value) {
        chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
        message->addStringValue(echo_test_key.c_str(), echo_test_value);
        return callApi(message);
    };
};

class TestMonitorConsumer:
public QuantumSlotConsumer {
    void quantumSlotHasData(const std::string& key,
                            const KeyValue& value) {
        //print fetched data
        std::cout << key <<  " - " << value->getJSONString() << "\n" << std::flush;
    };
    void quantumSlotHasNoData(const std::string& key) {
        //print fetched data
        std::cout << key << " - " << "No Data" <<std::endl;
    };
};

class StatusHandler:
public chaos::metadata_service_client::monitor_system::handler::HealtStatusAttributeHandler
{
    
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const std::string& value) {
        MSCT_INFO << "key:" << key << " attribute:" << attribute << " value:" << value;
    }
    
};

class HearbeatHandler:
public chaos::metadata_service_client::monitor_system::handler::HealtHeartBeatAttributeHandler
{
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value) {
        MSCT_INFO << "Tag:"<<tag<<" key:" << key << " attribute:" << attribute << " value:" << (uint64_t)value;
    }
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute) {
        MSCT_INFO << "Tag:"<<tag<<" key:" << key << " attribute:" << attribute << " no value found";
    }
public:
    std::string tag;
};

class BinaryHandler:
public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeBinaryHandler
{
public:
    BinaryHandler(const std::string& _attribute,
                  bool _event_on_value_change = false):
    QuantumKeyAttributeBinaryHandler(_attribute,
                                     _event_on_value_change){}
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const boost::shared_ptr<chaos::common::data::SerializationBuffer>& buffer) {
        MSCT_INFO << "Tag:"<<tag<<" key:" << key << " attribute:" << attribute << " with size:"<<buffer->getBufferLen();
    }
public:
    std::string tag;
};


void asyncTest(EchoTestProxy *echo_proxy_test) {
    
    uint64_t num_of_cicle = 0;
    for(int idx = 0; idx < 10000; idx++) {
        if(((num_of_cicle++) % 100) == 0) {
            std::cout << "." << std::flush;
        }
        std::string value = "value_echo_" + boost::lexical_cast<std::string>(global_counter++);
        ApiProxyResult r = echo_proxy_test->execute("key_echo", value);
        int i = 0;
        while (!r->wait()) {
            std::cout << "Waint for result pass:" << i++<< "\n" << std::flush;
            if(i>2) break;
        }
        
        if(r->getResult() != NULL) {
            //std::cout << r->getResult()->getJSONString() << "\n" << std::flush;
            assert(value.compare(r->getResult()->getStringValue("key_echo")) == 0);
        } else if(r->getError()){
            error_count++;
            //std::cerr << "Error code:"<<r->getError() << "\n" << std::flush;
            //std::cerr << "Error Message:"<<r->getErrorMessage() <<  "\n" << std::flush;
            //std::cerr << "Error Domain:"<<r->getErrorDomain() <<  "\n" << std::flush;
        } else {
            no_result_count++;
            //std::cerr << "No result found";
        }
        //sleep for 100ms
        //usleep(1000);
    }
}

int main(int argc, char * argv[]) {
    boost::thread_group tg;
    unsigned int quantum_multiplier;
    unsigned int wait_seconds;
    std::string device_id;
    TestMonitorConsumer *node_consumer = NULL;
    
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >("device-id",
                                                                                                          "Specify the device",
                                                                                                          &device_id);
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >("qm",
                                                                                                           "Specify the quantum multiplier to use",
                                                                                                           &quantum_multiplier);
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >("monitor-timeout",
                                                                                                           "Specify the time that we need to monitor the device in seconds",
                                                                                                           &wait_seconds);
    try{

        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        
        if(!device_id.size()) CHAOS_EXCEPTION(-1, "Invalid device id")
            
        ChaosMetadataServiceClient::getInstance()->start();
        
        ChaosMetadataServiceClient::getInstance()->enableMonitor();
        
        node_consumer = new TestMonitorConsumer();
        ChaosMetadataServiceClient::getInstance()->addKeyConsumer(device_id,
                                                                  quantum_multiplier,
                                                                  node_consumer);
        sleep(2);
        ChaosMetadataServiceClient::getInstance()->reconfigureMonitor();
        sleep(wait_seconds);
        ChaosMetadataServiceClient::getInstance()->removeKeyConsumer(device_id,
                                                                     quantum_multiplier,
                                                                     node_consumer);
        
        ChaosMetadataServiceClient::getInstance()->disableMonitor();
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    }catch(chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }catch(...) {
        std::cerr << "Unrecognized error";
    }
    if(node_consumer) delete(node_consumer);
    return 0;
}
