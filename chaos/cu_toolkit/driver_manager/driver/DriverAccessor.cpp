/*
 *	DriverAccessor.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>

using namespace chaos::cu::driver_manager::driver;

/*------------------------------------------------------
 
 ------------------------------------------------------*/
DriverAccessor::DriverAccessor(uint _accessorIndex):accessorIndex(_accessorIndex) {
    messagesCount = 0;
    
    //Allocate async message queue
    std::string randomUUID = UUIDUtil::generateUUID();
    accessorAsyncMQ = new boost::interprocess::message_queue(boost::interprocess::open_or_create    // open or create
                                                        ,randomUUID.c_str()                         // name queue  with casual UUID
                                                        ,100                                        // max trigger message
                                                        ,sizeof(MQAccessorResponseMessageType)      // dimension of the message
                                                        );
    driverAsyncMQ = new boost::interprocess::message_queue(boost::interprocess::open_only           // open or create
                                                      ,randomUUID.c_str()                           // name queue  with casual UUID
                                                      );
    
    //Allocate sync message queue
    randomUUID = UUIDUtil::generateUUID();
    accessorSyncMQ = new boost::interprocess::message_queue(boost::interprocess::open_or_create     // open or create
                                                            ,randomUUID.c_str()                     // name queue  with casual UUID
                                                            ,100                                    // max trigger message
                                                            ,sizeof(MQAccessorResponseMessageType)  // dimension of the message
                                                            );
    driverSyncMQ = new boost::interprocess::message_queue(boost::interprocess::open_only            // open or create
                                                          ,randomUUID.c_str()                       // name queue  with casual UUID
                                                          );

}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
DriverAccessor::~DriverAccessor() {
    //delete async message queue
    if(accessorAsyncMQ)  delete(accessorAsyncMQ);
    if(driverAsyncMQ) delete(driverAsyncMQ);
    
    //delete sync message queue
    if(accessorSyncMQ)  delete(accessorSyncMQ);
    if(driverSyncMQ) delete(driverSyncMQ);

}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::send(DrvMsgPtr cmd, uint priority) {
    CHAOS_ASSERT(cmd)

    unsigned int messagePriority = 0;
    MQAccessorResponseMessageType answerMessage = 0;
    boost::interprocess::message_queue::size_type recvd_size = 0;
    boost::interprocess::message_queue::size_type sent_size = sizeof(MQAccessorResponseMessageType);
    
    //fill the cmd with the information for retrive it
    cmd->id = messagesCount++;
    cmd->drvResponseMQ = driverSyncMQ;
    
    //send command
    commandQueue->send(&cmd, sizeof(DrvMsgPtr), priority);
    
    //whait the answer
    accessorSyncMQ->receive(&answerMessage, sent_size, recvd_size, messagePriority);
    
    //check result
    return recvd_size == sent_size;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::sendAsync(DrvMsgPtr cmd, MQAccessorResponseMessageType& messageID, uint priority) {
    CHAOS_ASSERT(cmd)
    
    //fill the cmd with the information for retrive it
    cmd->id = messageID = messagesCount++;
    cmd->drvResponseMQ = driverAsyncMQ;
    
    //send message
    commandQueue->send(&cmd, sizeof(DrvMsgPtr), priority);
    return true;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::getLastAsyncMsg(MQAccessorResponseMessageType& messageID) {
    unsigned int messagePriority = 0;
    boost::interprocess::message_queue::size_type recvd_size = 0;
    boost::interprocess::message_queue::size_type sent_size = sizeof(MQAccessorResponseMessageType);
    bool result = accessorAsyncMQ->try_receive(&messageID, sent_size, recvd_size, messagePriority);
    return result && recvd_size == sent_size;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::operator== (const DriverAccessor &a) {
    return this->accessorIndex = a.accessorIndex;
}