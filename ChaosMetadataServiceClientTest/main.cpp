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
                            KeyValue value) {
        //print fetched data
        MSCT_INFO << key << " - " << value->getJSONString();
    };
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
        TestMonitorConsumer test_consumer;
        global_counter = 0;
        error_count = 0;
        no_result_count=0;
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        ChaosMetadataServiceClient::getInstance()->start();

        ChaosMetadataServiceClient::getInstance()->addServerAddress("localhost:5000");

        ChaosMetadataServiceClient::getInstance()->enableMonitoring();
        
        ChaosMetadataServiceClient::getInstance()->addKeyConsumer("rt-claudio-1_healt",
                                                                  5,
                                                                  &test_consumer);

        
        //EchoTestProxy *echo_proxy_test = ChaosMetadataServiceClient::getInstance()->getApiProxy<EchoTestProxy>(1000);
        //tg.add_thread(new boost::thread(&asyncTest, echo_proxy_test));
        //tg.add_thread(new boost::thread(&asyncTest, echo_proxy_test));
        //tg.join_all();
        std::cout << "\nTest finisched with 2 thread\n" << std::flush;
        std::cout << "global_counter"<<global_counter<<"\n" << std::flush;
        std::cout << "error_count"<<error_count<<"\n" << std::flush;
        std::cout << "no_result_count"<<no_result_count<<"\n" << std::flush;
        sleep(600);
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    }catch(chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }catch(...) {
        std::cerr << "Unrecognized error";
    }
    return 0;
}
