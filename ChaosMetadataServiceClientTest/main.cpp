//
//  main.cpp
//  ChaosMetadataServiceClientTest
//
//  Created by Claudio Bisegni on 27/03/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <iostream>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

using namespace chaos::metadata_service_client;
int main(int argc, char * argv[]) {
    
    try{
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        ChaosMetadataServiceClient::getInstance()->start();

        GetNodeDescription *get_node_description_api = ChaosMetadataServiceClient::getInstance()->getApiProxy<GetNodeDescription>();

        for(int idx = 0; idx < 3; idx++) {
            ApiProxyResult r = get_node_description_api->execute("btw-transfer-line");
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
