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
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

using namespace chaos::metadata_service_client;

class EchoTestProxy:
public chaos::metadata_service_client::api_proxy::ApiProxy {
    API_PROXY_CLASS(EchoTestProxy)
protected:
    //! default constructor
    EchoTestProxy(chaos::common::message::MultiAddressMessageChannel *_mn_message):
    ApiProxy("echo", _mn_message){};
    //! default destructor
    ~EchoTestProxy(){};
public:
    
    /*!
     Return the description of the node
     */
    ApiProxyResult execute(const std::string& echo_test_key,
                           const std::string& echo_test_value) {
        chaos::common::data::CDataWrapper message;
        message.addStringValue(echo_test_key.c_str(), echo_test_value);
        return callApi("test", getName(), &message);
    };
};



int main(int argc, char * argv[]) {
    
    try{
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        ChaosMetadataServiceClient::getInstance()->start();

        EchoTestProxy *echo_proxy_test = ChaosMetadataServiceClient::getInstance()->getApiProxy<EchoTestProxy>();

        for(int idx = 0; idx < 3; idx++) {
            ApiProxyResult r = echo_proxy_test->execute("key_echo", "value_echo");
            int i = 0;
            while (!r->wait(1000)) {
                std::cout << "Waint for result pass:" << i++<< "\n" << std::flush;
                if(i>2) break;
            }

            if(r->getResult() != NULL) {
                std::cout << r->getResult()->getJSONString() << "\n" << std::flush;
            } else if(r->getError()){
                std::cerr << "Error code:"<<r->getError() << "\n" << std::flush;
                std::cerr << "Error Message:"<<r->getErrorMessage() <<  "\n" << std::flush;
                std::cerr << "Error Domain:"<<r->getErrorDomain() <<  "\n" << std::flush;
            } else {
                std::cerr << "No result found";
            }
        }

        
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    }catch(chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }catch(...) {
        std::cerr << "Unrecognized error";
    }
    return 0;
}
