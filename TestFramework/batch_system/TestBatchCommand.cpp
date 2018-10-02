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

#include "TestBatchCommand.h"
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;

TestBatchCommand::TestBatchCommand():
create_ts(TimingUtil::getTimeStampInMicroseconds()),
set_ts(0),
end_ts(0),
cicle_count(0){}

TestBatchCommand::~TestBatchCommand() {}

void TestBatchCommand::setHandler(CDataWrapper *data) {
    set_ts = TimingUtil::getTimeStampInMicroseconds();
    
    //se cicle to 10 usec
    setFeatures(chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)10);
    setFeatures(chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}
void TestBatchCommand::acquireHandler() {
    
    cicle_count++;
}

void TestBatchCommand::ccHandler() {
    if(cicle_count > 100) {BC_END_RUNNING_PROPERTY;}
}

bool TestBatchCommand::timeoutHandler() {
    end_ts = TimingUtil::getTimeStampInMicroseconds();
    BC_END_RUNNING_PROPERTY;
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
