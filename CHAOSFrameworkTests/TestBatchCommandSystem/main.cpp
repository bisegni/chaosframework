//
//  main.cpp
//  TestBatchCommandSystem
//
//  Created by Claudio Bisegni on 27/08/2016.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestCommandExecutor.h"

using namespace chaos::common::utility;
using namespace chaos::common::batch_command::test;

int main(int argc, const char * argv[]) {
    TestCommandExecutor executor;
    
    StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__);
    StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
    uint64_t command_id;
    for(int idx = 0;
        idx < 60;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id);
    }
    sleep(60);
    StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);

    return 0;
}
