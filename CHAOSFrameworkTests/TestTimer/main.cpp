//
//  main.cpp
//  TestTimer
//
//  Created by bisegni on 03/04/2017.
//  Copyright © 2017 bisegni. All rights reserved.
//

#include <iostream>

#include <chaos/common/async_central/async_central.h>

#include "TimerTask.h"

using namespace chaos::common::utility;
using namespace chaos::common::async_central;

int outer_counter = 0;
int internal_counter = 0;
int main(int argc, const char * argv[]) {
    InizializableService::initImplementation(AsyncCentralManager::getInstance(), NULL, "main", __PRETTY_FUNCTION__);
    for(int idx = 0;
        idx < 1000;
        idx++) {
        TimerTask task(internal_counter);
        AsyncCentralManager::getInstance()->addTimer(&task,
                                                     0,
                                                     1000);
        //usleep(10000);
        try{
            AsyncCentralManager::getInstance()->removeTimer(&task);
        }catch(boost::system::error_code &ex){
            std::cout << "Error on remove timer" << std::endl;
        }
    }
    InizializableService::deinitImplementation(AsyncCentralManager::getInstance(), "main", __PRETTY_FUNCTION__);
    return 0;
}
