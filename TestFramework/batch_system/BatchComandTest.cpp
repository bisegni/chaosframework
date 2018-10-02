/*
 * Copyright 2012, 02/10/2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include "BatchComandTest.h"

#include <chaos/common/log/LogManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
#define BATCH_COMMAND_INSTANCER(BatchCommandClass) new NestedObjectInstancer<TestBatchCommand, BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, TestBatchCommand>())

void BatchCommandTest::whaitOrAssertOnFault(TestCommandExecutor &executor, const unsigned int n_element) {
    BatchCommandStat s;
    do{
        sleep(1);
    }while(executor.getQueued() != 0 ||
           executor.getStacked() != 0 ||
           (executor.completed_count+executor.fault_count+executor.killed_count) != n_element);
}
TEST_F(BatchCommandTest, BatchCommandNormalSubmition) {
    uint64_t command_id = 0;
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    ASSERT_NO_THROW(StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__););
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    
    ASSERT_NO_THROW(StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_NORMAL);
    }
    whaitOrAssertOnFault(executor, 1000);
    ASSERT_EQ(executor.completed_count, 1000);
    ASSERT_EQ(executor.fault_count, 0);
    ASSERT_EQ(executor.killed_count, 0);
    ASSERT_NO_THROW(StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
}
TEST_F(BatchCommandTest, BatchCommandStackSubmition) {
    uint64_t command_id = 0;
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    ASSERT_NO_THROW(StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__););
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    
    ASSERT_NO_THROW(StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    
    //test stack
    for(int idx = 0;
        idx < 1000;
        idx++) {
        
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, chaos::common::batch_command::SubmissionRuleType::SUBMIT_AND_STACK);
    }
    whaitOrAssertOnFault(executor, 1000);
    ASSERT_EQ(executor.completed_count, 1000);
    ASSERT_EQ(executor.fault_count, 0);
    ASSERT_EQ(executor.killed_count, 0);
    ASSERT_NO_THROW(StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
}
TEST_F(BatchCommandTest, BatchCommandRandomSubmition) {
    uint64_t command_id = 0;
    std::srand((unsigned int)std::time(0));
    TestCommandExecutor executor;
    ASSERT_NO_THROW(StartableService::initImplementation(executor, NULL, "TestCommandExecutor", __PRETTY_FUNCTION__););
    
    executor.installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    executor.installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
    
    ASSERT_NO_THROW(StartableService::startImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    //test random
    for(int idx = 0;
        idx < 1000;
        idx++) {
        executor.submitCommand("TestCommandComplete", NULL, command_id, 0, 100, static_cast<chaos::common::batch_command::SubmissionRuleType::SubmissionRule>(std::rand()%3));
    }
    
    whaitOrAssertOnFault(executor, 1000);
    
    ASSERT_NO_THROW(StartableService::stopImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(StartableService::deinitImplementation(executor, "TestCommandExecutor", __PRETTY_FUNCTION__););
}
