/*
 *	DriverAccessor.cpp
 *	!CHAOS
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
DriverAccessor::DriverAccessor(uint _accessor_index):
    accessor_index(_accessor_index),
    messages_count(0),
    accessor_async_mq(new AccessorQueueType()),
    accessor_sync_mq(new AccessorQueueType()),
    base_opcode_priority(50) {}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
DriverAccessor::~DriverAccessor() {
    //delete async message queue
    if(accessor_async_mq)  {delete(accessor_async_mq);}
    
    //delete sync message queue
    if(accessor_sync_mq)  {delete(accessor_sync_mq);}

}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::send(DrvMsgPtr cmd,
                          uint32_t inc_priority) {
    CHAOS_ASSERT(cmd)

    ResponseMessageType answer_message = 0;
    
    //fill the cmd with the information for retrieve it
    cmd->id = messages_count++;
    cmd->drvResponseMQ = accessor_sync_mq;
    
    //send command
    command_queue->push(cmd, base_opcode_priority + inc_priority);
    //whait the answer
    accessor_sync_mq->wait_and_pop(answer_message);
    
    //check result
    return (answer_message == MsgManagmentResultType::MMR_EXECUTED);
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::sendAsync(DrvMsgPtr cmd, ResponseMessageType& message_id, uint32_t inc_priority) {
    CHAOS_ASSERT(cmd)
    
    //fill the cmd with the information for retrive it
    cmd->id = message_id = messages_count++;
    cmd->drvResponseMQ = accessor_async_mq;
    
    //send message
	command_queue->push(cmd, base_opcode_priority + inc_priority);
    return true;
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::getLastAsyncMsg(ResponseMessageType& message_id) {
    return accessor_sync_mq->try_pop(message_id);
}

/*------------------------------------------------------
 
 ------------------------------------------------------*/
bool DriverAccessor::operator== (const DriverAccessor &a) {
    return this->accessor_index = a.accessor_index;
}
