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

int main(int argc, const char * argv[]) {
    InizializableService::initImplementation(AsyncCentralManager::getInstance(), NULL, "main", __PRETTY_FUNCTION__);
    for(int idx = 0;
        idx < 1000;
        idx++) {
        TimerTask task;
        AsyncCentralManager::getInstance()->addTimer(&task,
                                                     0,
                                                     1);
        usleep(10000);
        AsyncCentralManager::getInstance()->removeTimer(&task);
    }
    InizializableService::deinitImplementation(AsyncCentralManager::getInstance(), "main", __PRETTY_FUNCTION__);
    return 0;
}
