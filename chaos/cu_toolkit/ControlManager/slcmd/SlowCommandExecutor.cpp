/*
 *	SlowCommandExecutor.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include "SlowCommandExecutor.h"


using namespace chaos::cu::cm::slcmd;

SlowCommandExecutor::SlowCommandExecutor(AbstractControlUnit *_controUnitReference):controUnitReference(_controUnitReference) {
    
}

SlowCommandExecutor::~SlowCommandExecutor() {
    
}

// Initialize instance
void SlowCommandExecutor::init(void*) throw(chaos::CException) {
}


// Start the implementation
void SlowCommandExecutor::start() throw(chaos::CException) {
    // thread creation
    incomingCheckThreadPtr.reset(new boost::thread(boost::bind(&SlowCommandExecutor::performIncomingCommandCheck, this)));
#if defined(__linux__) || defined(__APPLE__)
    int retcode;
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) incomingCheckThreadPtr->native_handle();
    if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)  {
        throw CException(retcode, "Get Thread schedule param", "SlowCommandExecutor::init");
    }
    
    
    policy = SCHED_RR;
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0) {
        throw CException(retcode, "Set Thread schedule param", "SlowCommandExecutor::init");
    }
#endif
}

// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    //lock for queue access
    boost::unique_lock<boost::mutex> lock(queueMutext);
}

// Deinit the implementation
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    
}

//! Perform a command registration
void SlowCommandExecutor::setDefaultCommand(const char *defaultCommandAlias) {
    
}

//! Check the incoming command rule
void SlowCommandExecutor::performIncomingCommandCheck() {
    //lock for queue access
    boost::unique_lock<boost::mutex> lock(queueMutext);
    
    bool canBeUsed = false;
    
    PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
    //-------------------check if we can use the command-------------------------
     boost::posix_time::time_duration delay(0,0,0,0);
    while(commandSubmittedQueue.empty() && !incomingCheckThreadPtr->timed_join(delay)) {
        emptyQueueConditionLock.notify_one();
        readThreadWhait.wait(lock);
    }
    //-------------------check if we can use the command-------------------------
    //remove commando form the queue
    if(canBeUsed) commandSubmittedQueue.pop();
}


//! Submite the new sloc command information
bool SlowCommandExecutor::submitCommandInfo(CDataWrapper *commandDescription, uint8_t priority, bool disposeOnDestroy) {
    boost::unique_lock<boost::mutex> lock(queueMutext);
    if(serviceState != utility::StartableServiceType::SS_STARTED) return false;
    PriorityQueuedElement<CDataWrapper> *_element = new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, disposeOnDestroy);
    commandSubmittedQueue.push(_element);
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}