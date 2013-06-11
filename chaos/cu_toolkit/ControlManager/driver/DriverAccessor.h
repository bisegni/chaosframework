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
#include <chaos/cu_toolkit/ControlManager/driver/DriverGlobal.h>

namespace chaos{
    
    //! forward declaration
    template <typename T>
    class PriorityQueuedElement;
    
    namespace cu {
        namespace cm {
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
                    
                    //! Number of the command sent
                    /*!
                        This index is updated avery time a command is issued to the driver. It
                        used for check whenever the driver has finiscehd to execute the command
                        related to it.
                     */
                    boost::atomic_uint64_t messagesCount;
                    
                    //! Asynchronous accessor queue
                    /*!
                     This queue is used by the accessor to check whenever
                     a command, identified by his id, has been processed.
                     */
                    boost::interprocess::message_queue *accessorAsyncMQ;
                    
                    //! Asynchronous driver queue
                    /*!
                        This queue is forwarded to the driver, within the
                        command pack, to permit to it to give the response
                        to an synchronous command.
                     */
                    boost::interprocess::message_queue *driverAsyncMQ;
                    
                    //! Synchronous accessor queue
                    /*!
                     This queue is used by the accessor to Synchronous check whenever
                     a command, identified by his id, has been processed.
                     */
                    boost::interprocess::message_queue *accessorSyncMQ;
                    
                    //! Synchronous driver queue
                    /*!
                     This queue is forwarded to the driver, within the
                     command pack, to permit to it to give the response
                     to an synchronous command.
                     */
                    boost::interprocess::message_queue *driverSyncMQ;
                    
                    //! input driver shared queue
                    /*!
                     This queue is to forward command to the driver
                     */
                    boost::interprocess::message_queue *commandQueue;
                    
                    //Private constructor
                    DriverAccessor();
                    
                public:

                    //Private destructor
                    ~DriverAccessor();
                    
                    //! Send synchronous command
                    /*!
                        A command is sent to the driver and a synchronous
                        answere is waith from the driver.
                        \param cmd a command pack filled with all infromation
                                for the command.
                        \param priority the priority permit to be forward a message
                                before message with minor priority.
                        \return true if operation has been done sucessfull.
                     */
                    bool send(DrvMsgPtr cmd, uint priority = 0);
                    
                    //! Send an asynchronous command
                    /*!
                        A command is sent to the driver and a synchronous
                        answere is waith from the driver.
                        \param cmd a command pack filled with all infromation
                            for the command.
                        \param messageID is the code associated to the async command.
                        \param priority the priority permit to be forward a message
                            before message with minor priority.
                        \return true if operation has been done sucessfull.
                     */
                    bool sendAsync(DrvMsgPtr cmd, MQAccessorResponseMessageType& messageID, uint priority = 0);
                    
                    //! Check last processed code
                    /*!
                        The asynchronous queue is checked for a result code. If one
                        is found it s returned.
                     \param messageID is filled with the found code.
                     \return true if a code has been found.
                     */
                    bool getLastAsyncMsg(MQAccessorResponseMessageType& messageID);

                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DriverAccessor__) */
