/*
 * Copyright 2012, 2017 INFN
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

#include "TestBatchDefault.h"
using namespace chaos::common::data;
TestBatchDefaultCommand::TestBatchDefaultCommand(){}

TestBatchDefaultCommand::~TestBatchDefaultCommand() {}

void TestBatchDefaultCommand::setHandler(CDataWrapper *data) {
    setFeatures(chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
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
    return chaos::common::batch_command::HandlerType::HT_Set|
    chaos::common::batch_command::HandlerType::HT_Correlation|
    chaos::common::batch_command::HandlerType::HT_Acquisition;
}
