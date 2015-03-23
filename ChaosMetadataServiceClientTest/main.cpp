//
//  main.cpp
//  ChaosMetadataServiceClientTest
//
//  Created by Claudio Bisegni on 27/03/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <iostream>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;
int main(int argc, char * argv[]) {
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);
    return 0;
}
