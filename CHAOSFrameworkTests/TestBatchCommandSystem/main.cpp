//
//  main.cpp
//  TestBatchCommandSystem
//
//  Created by Claudio Bisegni on 27/08/2016.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestCommandExecutor.h"


#include <chaos/common/log/LogManager.h>


using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::batch_command::test;

#define BATCH_COMMAND_INSTANCER(BatchCommandClass) new NestedObjectInstancer<TestBatchCommand, BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, TestBatchCommand>())

void whaitOrAssertOnFault(TestCommandExecutor &executor, bool has_default) {
    BatchCommandStat s;
    do{
        s = executor.getStat();
        std::cout << "In stack:" << s.stacked_commands << std::endl;
        std::cout << "In queue:" << s.queued_commands << std::endl;
        sleep(1);
    }while(s.queued_commands == 0 &&
           s.stacked_commands == 0);
}

void testInfrastructure() {
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    executor.installCommand("TestBatchDefaultCommand", BATCH_COMMAND_INSTANCER(TestBatchDefaultCommand));
    //executor.has_default = true;
    //executor.setDefaultCommand("TestBatchDefaultCommand");
    std::cout << "--------------Testing architecture--------------"<<std::endl;
    //test sequence normal
    uint64_t command_id;
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_NORMAL);
    }
    
    //test stack
    for(int idx = 0;
        idx < 1000;
        idx++) {
        
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_AND_STACK);
    }
    
    //test random
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, static_cast<chaos::common::batch_command::SubmissionRuleType::SubmissionRule>(std::rand()%3));
    }
    
    StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
    whaitOrAssertOnFault(executor, true);
    
    executor.printStatistic();
    
    StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
}
void benchmarkInfrastructure() {
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    executor.installCommand("TestBatchDefaultCommand", BATCH_COMMAND_INSTANCER(TestBatchDefaultCommand));
    executor.has_default = true;
    executor.setDefaultCommand("TestBatchDefaultCommand");
    std::cout << "--------------Benchmarking architecture--------------"<<std::endl;
    //test sequence normal
    uint64_t command_id;
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_NORMAL);
    }
    
    StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    whaitOrAssertOnFault(executor, true);
    
    executor.printStatistic();
    
    StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
}
int main(int argc, const char * argv[]) {
    chaos::common::log::LogManager::getInstance()->init();
    testInfrastructure();
    benchmarkInfrastructure();
    return 0;
}
