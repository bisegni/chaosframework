//
//  main.cpp
//  TestBatchCommandSystem
//
//  Created by Claudio Bisegni on 27/08/2016.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestCommandExecutor.h"


#include <chaos/common/log/LogManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::batch_command::test;

#define BATCH_COMMAND_INSTANCER(BatchCommandClass) new NestedObjectInstancer<TestBatchCommand, BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, TestBatchCommand>())

void whaitOrAssertOnFault(TestCommandExecutor &executor, const unsigned int n_element) {
    BatchCommandStat s;
    do{
        std::cout << "In stack:" << executor.getStacked() << std::endl;
        std::cout << "In queue:" << executor.getQueued() << std::endl;
        std::cout << "In completed_count:" << executor.completed_count<<std::endl;
        std::cout << "In fault_count:" << executor.fault_count<<std::endl;
        std::cout << "In killed_count:" << executor.killed_count<<std::endl;
        sleep(1);
    }while(executor.getQueued() != 0 ||
           executor.getStacked() != 0 ||
           (executor.completed_count+executor.fault_count+executor.killed_count) != n_element);
    std::cout << "End with --------------"<<std::endl;
    std::cout << "In stack:" << executor.getStacked() << std::endl;
    std::cout << "In queue:" << executor.getQueued() << std::endl;
    std::cout << "In completed_count:" << executor.completed_count<<std::endl;
    std::cout << "In fault_count:" << executor.fault_count<<std::endl;
    std::cout << "In killed_count:" << executor.killed_count<<std::endl;
}

void testInfrastructure() {
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    
    StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    
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
    whaitOrAssertOnFault(executor, 1000);
    
    executor.resetStat();
    //test stack
    for(int idx = 0;
        idx < 1000;
        idx++) {
        
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_AND_STACK);
    }
    whaitOrAssertOnFault(executor, 1000);
     executor.resetStat();
    //test random
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, static_cast<chaos::common::batch_command::SubmissionRuleType::SubmissionRule>(std::rand()%3));
    }
    
    whaitOrAssertOnFault(executor, 1000);
    
    executor.printStatistic();
    
    StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
    StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__);
}

int main(int argc, char ** argv) {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
    chaos::common::log::LogManager::getInstance()->init();
    testInfrastructure();
    return 0;
}
