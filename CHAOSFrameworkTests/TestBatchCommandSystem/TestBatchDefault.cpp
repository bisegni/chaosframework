/*
 *	TestBatchDefault.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 29/08/16 INFN, National Institute of Nuclear Physics
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

#include "TestBatchDefault.h"
using namespace chaos::common::batch_command::test;
TestBatchDefaultCommand::TestBatchDefaultCommand(){}

TestBatchDefaultCommand::~TestBatchDefaultCommand() {}

void TestBatchDefaultCommand::setHandler(CDataWrapper *data) {
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
}
void TestBatchDefaultCommand::acquireHandler() {
    std::cout << "Default acquire" << std::endl;
}

void TestBatchDefaultCommand::ccHandler() {
    std::cout << "Default correlation" << std::endl;
    
}

bool TestBatchDefaultCommand::timeoutHandler() {
    return false;
}

uint8_t TestBatchDefaultCommand::implementedHandler() {
    return HandlerType::HT_Set|
    HandlerType::HT_Correlation|
    HandlerType::HT_Acquisition;
}
