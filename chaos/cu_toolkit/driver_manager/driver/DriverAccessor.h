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

#ifndef __CHAOSFramework__DriverAccessor__
#define __CHAOSFramework__DriverAccessor__

#include <set>
#include <boost/atomic/atomic.hpp>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/common/thread/TemplatedConcurrentQueue.h>

namespace chaos_thread_ns = chaos::common::thread;

namespace chaos{
    
    // forward declaration
    template <typename T>
    class PriorityQueuedElement;
    
    namespace cu {
        namespace driver_manager {
			
			//forward declaration
			class DriverManager;
			
            namespace driver {
                
                
                
                //! Driver accessor comminication class
                /*!
                    The accessor class represent the way used by another class for communiate with the driver.
                    Three queue are used; two for receive sync and async result, and another to send command 
                    to the driver.
                 */
                class DriverAccessor {
                    
                    //make driver friendly
                    friend class AbstractDriver;
                    friend class chaos::cu::driver_manager::DriverManager;
					
                    //! idnetificaiton of the driver that has created the accessor
					std::string driver_uuid;
					
                    //! index of the accessor for the driver
                    uint accessor_index;

                    
                    //! Number of the command sent
                    /*!
                        This index is updated avery time a command is issued to the driver. It
                        used for check whenever the driver has finiscehd to execute the command
                        related to it.
                     */
                    boost::atomic_uint64_t messages_count;
                    
                    //! Asynchronous accessor queue
                    /*!
                     This queue is used by the accessor to check whenever
                     a command, identified by his id, has been processed.
                     */
                    AccessorQueueType *accessor_async_mq;

                    
                    //! Synchronous accessor queue
                    /*!
                     This queue is used by the accessor to Synchronous check whenever
                     a command, identified by his id, has been processed.
                     */
                    AccessorQueueType *accessor_sync_mq;
                    
                    //! input driver shared queue
                    /*!
                     This queue is to forward command to the driver
                     */
                    //boost::interprocess::message_queue *commandQueue;
                    DriverQueueType *command_queue;
                    
                    //Private constructor
                    DriverAccessor(uint _accessor_index);
                    
                    //Private destructor
                    ~DriverAccessor();
                    
                public:
                    
                    //! base priority of the accessor(the default value is 50)
                    uint32_t base_opcode_priority;
                    
                    //! Send synchronous command
                    /*!
                        A command is sent to the driver and a synchronous
                        answere is waith from the driver.
                        \param cmd a command pack filled with all infromation
                                for the command.
                        \param inc_priority the incremental priority repsect to the base that permit to be forward a message
                                before message with minor priority.
                        \return true if operation has been done sucessfull.
                     */
                    bool send(DrvMsgPtr cmd, uint32_t inc_priority = 0);
                    
                    //! Send an asynchronous command
                    /*!
                        A command is sent to the driver and a synchronous
                        answere is waith from the driver.
                        \param cmd a command pack filled with all infromation
                            for the command.
                        \param message_id is the code associated to the async command.
                        \param inc_priority the incremental priority repsect to the base that permit to be forward a message
                            before message with minor priority.
                        \return true if operation has been done sucessfull.
                     */
                    bool sendAsync(DrvMsgPtr cmd, ResponseMessageType& message_id, uint32_t inc_priority = 0);
                    
                    //! Check last processed code
                    /*!
                        The asynchronous queue is checked for a result code. If one
                        is found it s returned.
                     \param message_id is filled with the found code.
                     \return true if a code has been found.
                     */
                    bool getLastAsyncMsg(ResponseMessageType& message_id);
					
                    //! Equals operator overloading
                    bool operator== (const DriverAccessor &a);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DriverAccessor__) */
