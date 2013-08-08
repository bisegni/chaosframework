/*
 *	AbstractDriver.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <string>
#include <sched.h>

#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

using namespace chaos::cu::driver_manager::driver;

#define ADLAPP_ LAPP_ << "[AbstractDriver-" << "] "
#define ADLDBG_ LDBG_ << "[SlowCommandSandbox-" << "] "
#define ADLERR_ LERR_ << "[SlowCommandSandbox-" << "] "

/*------------------------------------------------------

------------------------------------------------------*/
AbstractDriver::AbstractDriver() {
    accessorCount = 0;
    driverUUID = chaos::UUIDUtil::generateUUID();
    commandQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create         // open or create
                                                          ,driverUUID.c_str()                         // name queue  with casual UUID
                                                          ,1000                                       // max driver message queue
                                                          ,sizeof(drvqueuedata_t));                 // dimension of the message
                                                          
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
AbstractDriver::~AbstractDriver() {
    if(commandQueue) delete(commandQueue);
}

// Initialize instance
void AbstractDriver::init(void *initParamPtr) throw(chaos::CException) {
	DrvMsg initMsg;
	std::memset(&initMsg, 0, sizeof(DrvMsg));
	
	const char *initStr = (const char *)initParamPtr;
	
	initMsg.opcode = OpcodeType::OP_INIT;
	initMsg.data = initParamPtr;
	initMsg.dataLength = (uint32_t)strlen(initStr);
    if(execOpcode(&initMsg) == MsgManagmentResultType::MMR_INIT_ERROR) {
		throw CException(1, "Driver inititalization error", "AbstractDriver::init");
	}
	
	//start interna thread for the waithing of the message
	threadMessageReceiver.reset(new boost::thread(boost::bind(&AbstractDriver::scanForMessage, this)));
	
	//set the scheduler thread priority
#if defined(__linux__) || defined(__APPLE__)
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) threadMessageReceiver->native_handle();
    if (!pthread_getschedparam(threadID, &policy, &param))  {
		DEBUG_CODE(ADLAPP_ << "Default thread scheduler policy";)
        DEBUG_CODE(ADLAPP_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
											 (policy == SCHED_RR)    ? "SCHED_RR" :
											 (policy == SCHED_OTHER) ? "SCHED_OTHER" :
											 "???");)
		DEBUG_CODE(ADLAPP_ << "priority " << param.sched_priority;)
		
		policy = SCHED_RR;
		param.sched_priority = sched_get_priority_max(SCHED_RR);
		if (!pthread_setschedparam(threadID, policy, &param)) {
			//successfull setting schedule priority to the thread
			DEBUG_CODE(ADLAPP_ << "new thread scheduler policy";)
			DEBUG_CODE(ADLAPP_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
												 (policy == SCHED_RR)    ? "SCHED_RR" :
												 (policy == SCHED_OTHER) ? "SCHED_OTHER" :
												 "???");)
			DEBUG_CODE(ADLAPP_ << "priority " << param.sched_priority;)
			
		}
	}
#endif

}

// Deinit the implementation
void AbstractDriver::deinit() throw(chaos::CException) {
    DrvMsg deinitMsg;
	std::memset(&deinitMsg, 0, sizeof(DrvMsg));
	deinitMsg.opcode = OpcodeType::OP_DEINIT;
	
	//open a  temporary queue for send the deinit message to the thread
	auto_ptr<boost::interprocess::message_queue> deinitQueue(new boost::interprocess::message_queue(boost::interprocess::open_only,driverUUID.c_str()));
	//send deinit opcode to the queue
	drvqueuedata_t tmp = (drvqueuedata_t)&deinitMsg;
	deinitQueue->send(&tmp, sizeof(drvqueuedata_t), 0);
	
	//now join to  the thread if joinable
	if(threadMessageReceiver->joinable()) threadMessageReceiver->join();
	
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool AbstractDriver::getNewAccessor(DriverAccessor **newAccessor) {
    
    //allocate new accessor;
    DriverAccessor *result = new DriverAccessor(accessorCount++);
    if(result) {
			//set the parent uuid
		result->driverUUID = driverUUID;
            //share the input queue ptr
        result->commandQueue = new boost::interprocess::message_queue(boost::interprocess::open_only    // open or create
                                                                      ,driverUUID.c_str());             // name queue  with casual UUID
        
        
        boost::unique_lock<boost::shared_mutex> lock(accessoListShrMux);
        accessors.push_back(result);
        lock.unlock();
        
        *newAccessor = result;
    }
    return result != NULL;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool AbstractDriver::releaseAccessor(DriverAccessor *accessor) {
    if(!accessor) return false;
    
	if(accessor->driverUUID.compare(driverUUID) != 0) {
		ADLERR_ << "has been requested to relase an accessor with uuid=" << accessor->driverUUID << "that doesn't belong to this driver with uuid =" << driverUUID;
		return false;
	}
    boost::unique_lock<boost::shared_mutex> lock(accessoListShrMux);
    accessors.erase(std::find(accessors.begin(), accessors.end(), accessor));
    lock.unlock();
    
    delete(accessor);
	return true;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
void AbstractDriver::scanForMessage() {
	ADLAPP_ << "scanForMessage thread started for uuid " << driverUUID;
    DrvMsgPtr currentMessagePtr;
	uintptr_t dataReceived;
    unsigned int messagePriority = 0;
    boost::interprocess::message_queue::size_type recvd_size = 0;
    boost::interprocess::message_queue::size_type sent_size = sizeof(drvqueuedata_t);
    do {
        //wait for the new command
        commandQueue->receive(&dataReceived, sent_size, recvd_size, messagePriority);
        
        //broadcast the execution
        execOpcode(currentMessagePtr = (DrvMsgPtr)dataReceived);
        
        //notify the caller
        if(currentMessagePtr->drvResponseMQ)
			currentMessagePtr->drvResponseMQ->send(&currentMessagePtr->id, sizeof(mq_accessor_response_message_t), messagePriority);
    } while (currentMessagePtr->opcode != OpcodeType::OP_DEINIT);
	ADLAPP_ << "scanForMessage thread ended for uuid " << driverUUID;
}