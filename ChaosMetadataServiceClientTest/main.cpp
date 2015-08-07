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
        MSCT_INFO << key << " - " << value->getJSONString() <<std::endl;
    };
    void quantumSlotHasNoData(const std::string& key) {
        //print fetched data
        MSCT_INFO << key << " - " << "No Data" <<std::endl;
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
    try{
        unsigned int quantum_multiplier;
        std::string device_id;
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >("device-id",
                                                                                                              "Specify the device",
                                                                                                              &device_id);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >("qm",
                                                                                                               "Specify the quantum multiplier to use",
                                                                                                               &quantum_multiplier);
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        
        if(!device_id.size()) CHAOS_EXCEPTION(-1, "Invalid device id")
        
        ChaosMetadataServiceClient::getInstance()->start();
        
        ChaosMetadataServiceClient::getInstance()->addServerAddress("pcbisegni.lnf.infn.it:5000");
        
        ChaosMetadataServiceClient::getInstance()->enableMonitoring();
        
        HearbeatHandler *hb_handler = new HearbeatHandler();
        for(int idx = 0; idx < 100; idx++) {
            ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(device_id, quantum_multiplier, hb_handler);
            usleep(500000);
            ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(device_id, quantum_multiplier, hb_handler);
        }
        delete(hb_handler);
        
        HearbeatHandler *hb_handlers[100];
        for(int idx = 0; idx < 100; idx++) {
            hb_handlers[idx] = new HearbeatHandler();
            hb_handlers[idx]->tag = boost::lexical_cast<std::string>(idx);
            ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(device_id, quantum_multiplier, hb_handlers[idx]);
        }
        
        usleep(5000000);
        
        for(int idx = 0; idx < 100; idx++) {
            ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(device_id, quantum_multiplier, hb_handlers[idx]);
            delete(hb_handlers[idx]);
        }
        ChaosMetadataServiceClient::getInstance()->disableMonitoring();
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    }catch(chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }catch(...) {
        std::cerr << "Unrecognized error";
    }
    return 0;
}
