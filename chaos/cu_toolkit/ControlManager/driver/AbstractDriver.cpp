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

#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/cu_toolkit/ControlManager/driver/AbstractDriver.h>
#include <chaos/cu_toolkit/ControlManager/driver/DriverAccessor.h>

using namespace chaos::cu::cm::driver;

/*------------------------------------------------------

------------------------------------------------------*/
AbstractDriver::AbstractDriver() {
    driverUUID = chaos::UUIDUtil::generateUUID();
    commandQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create         // open or create
                                                          ,driverUUID.c_str()                         // name queue  with casual UUID
                                                          ,1000                                       // max driver message queue
                                                          ,sizeof(MQAccessorMessageType)              // dimension of the message
                                                          );
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
AbstractDriver::~AbstractDriver() {
    
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool AbstractDriver::getNewAccessor(DriverAccessor **newAccessor) {
    
    //allocate new accessor;
    DriverAccessor *result = new DriverAccessor();
    if(result) {
            //share the input queue ptr
        result->commandQueue = new boost::interprocess::message_queue(boost::interprocess::open_only    // open or create
                                                                      ,driverUUID.c_str()               // name queue  with casual UUID
                                                                      );
    }
    return result != NULL;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
void AbstractDriver::scanForMessage() {
    DrvMsgPtr currentMessagePtr;
    unsigned int messagePriority = 0;
    boost::interprocess::message_queue::size_type recvd_size = 0;
    boost::interprocess::message_queue::size_type sent_size = sizeof(MQAccessorMessageType);
    do {
        //wait for the new command
        commandQueue->receive(&currentMessagePtr, sent_size, recvd_size, messagePriority);
        
        //broadcast the execution
        execOpcode(currentMessagePtr);
        
        //notify the caller
        currentMessagePtr->drvResponseMQ->send(&currentMessagePtr->id, sizeof(MQAccessorMessageType), messagePriority);
    } while (currentMessagePtr->opcode == OP_END);
    
}