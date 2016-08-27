/*
 *	TestBatchCommand.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/08/2016 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "TestBatchCommand.h"
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::batch_command::test;

TestBatchCommand::TestBatchCommand():
create_ts(TimingUtil::getTimeStamp()),
set_ts(0),
end_ts(0),
cicle_count(0){}

TestBatchCommand::~TestBatchCommand() {}

void TestBatchCommand::setHandler(CDataWrapper *data) {
    set_ts = TimingUtil::getTimeStamp();
    
    //se cicle to 10 usec
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)10);
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)1000000);
}
void TestBatchCommand::acquireHandler() {
    
    cicle_count++;
}

void TestBatchCommand::ccHandler() {
    
}

bool TestBatchCommand::timeoutHandler() {
    end_ts = TimingUtil::getTimeStamp();
    return false;
}

uint8_t TestCommandSetOnly::implementedHandler() {
    return HandlerType::HT_Set;
}


uint8_t TestCommandComplete::implementedHandler() {
    return HandlerType::HT_Set|
    HandlerType::HT_Correlation|
    HandlerType::HT_Acquisition;
}
