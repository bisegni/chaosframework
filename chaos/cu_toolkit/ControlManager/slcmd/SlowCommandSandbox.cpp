//
//  SlowCommandSandbox.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandSandbox.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandConstants.h>


using namespace chaos::cu::control_manager::slow_command;


//! Initialize instance
void SlowCommandSandbox::init(void*) throw(chaos::CException) {
    
}

//! Deinit the implementation
void SlowCommandSandbox::deinit() throw(chaos::CException) {
    
}


void SlowCommandSandbox::runCommand() {
    
}

void SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    
}

//! Check if the new command can be installed
bool SlowCommandSandbox::trySetupAvailableCommand() {
    // get the command submission info
    boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler);
    CDataWrapper *submissionInfo = nextAvailableCommand->cmdInfo->element;
    CHAOS_ASSERT(submissionInfo)
    uint32_t submissionFlag = SubmissionRuleType::SUBMIT_NORMAL;
        
    //check for the submission rule
    if(submissionInfo->hasKey(SlowCommandSubmissionKey::COMMAND_ALIAS)) {
        submissionFlag = submissionInfo->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_FLAG);
    }
    return true;
}