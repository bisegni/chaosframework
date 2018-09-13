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

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/control_manager/slow_command/command/SCWaitCommand.h>

using namespace chaos::common::data;
using namespace chaos::common::exception;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::control_manager::slow_command::command;

#define SCWC_INFO INFO_LOG(SCWaitCommand) << "[" <<getDeviceID() << " - " << getUID()<< "] "

#define SCWaitCommand_DELAY_KEY "delay"

//------------------command description------------------------------
BATCH_COMMAND_OPEN_DESCRIPTION(chaos::cu::control_manager::slow_command::command::,
                               SCWaitCommand,
                               "Default command that permit to wait a determinated amount of milliseconds",
                               "007F0151-2FC6-4C22-BFC1-2CC684C2C771")
BATCH_COMMAND_ADD_INT32_PARAM(BatchCommandsKey::DEFAULT_WAIT_COMMAND_DELAY_KEY,
                              "Is the number of milliseconds to wait untile end the command.",
                              BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()
//------------------command description------------------------------

// return the implemented handler
uint8_t SCWaitCommand::implementedHandler() {
    //we need to implements all handler because we need to pause all
    return  HandlerType::HT_Set |
    HandlerType::HT_Correlation;
    
}

// Start the command execution
void SCWaitCommand::setHandler(CDataWrapper *data) {
    if(!data) return;
    
    if(!data->hasKey(SCWaitCommand_DELAY_KEY)) {
        throw MetadataLoggingCException(getDeviceID(),
                                        -1,
                                        "No delay key found!",
                                        __FUNCTION__);
    }
    
    if(!data->isInt32Value(SCWaitCommand_DELAY_KEY)) {
        throw MetadataLoggingCException(getDeviceID(),
                                        -2,
                                        "delay key is not of int32 type",
                                        __FUNCTION__);
    }
    
    uint32_t delay = data->getUInt32Value(SCWaitCommand_DELAY_KEY);
    setFeatures(features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)delay);
    BC_EXCLUSIVE_RUNNING_PROPERTY;
    DEBUG_CODE(SCWC_INFO << "timeout set to " << delay << " milliseconds";)
}


void SCWaitCommand::ccHandler() {
    SCWC_INFO << CHAOS_FORMAT("Wait command runs for %1% msec",%(getStartStepTime()-getSetTime()));
}


bool SCWaitCommand::timeoutHandler() {
    uint64_t timeDiff = getStartStepTime() - getSetTime();
    DEBUG_CODE(SCWC_INFO << "timeout after " << timeDiff << " milliseconds";)
    BC_END_RUNNING_PROPERTY;
    //move the state machine on fault
    return false;
}
