//
//  main.cpp
//  ChaosHSTService
//
//  Created by Claudio Bisegni on 03/11/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "ChaosDataService.h"

using namespace chaos::data_service;

int main(int argc, char * argv[])
{
    
    ChaosDataService::getInstance()->init(argc, argv);
    ChaosDataService::getInstance()->start();
    return 0;
}

