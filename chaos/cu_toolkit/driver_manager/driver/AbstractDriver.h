/*
 *	AbstractDriver.h
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

#ifndef __CHAOSFramework__AbstractDriver__
#define __CHAOSFramework__AbstractDriver__

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/thread/TemplatedConcurrentQueue.h>

#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>


namespace chaos_thread_ns = chaos::common::thread;

namespace chaos{
    namespace cu {
        namespace driver_manager {
			
			//forward declaration
	  class DriverManager;
			
            namespace driver {
                
                //! forward declaration
                class DriverAccessor;
                template<typename T>
                class DriverWrapperPlugin;
                
				typedef struct DriverDescirption {
					std::string alias;
					std::string description;
					std::string version;
					std::string init_paramter_sintax;
				} DriverDescirption;
				
                    //! !CHAOS Device Driver abstract class
                /*!
                    This represent the base class for all driver in !CHAOS. For standardize the comunicacetion 
                    a message queue is used for receive DrvMsg pack.
                 */
				class AbstractDriver:
				public chaos::common::utility::InizializableService {
                    template<typename T>
                    friend class DriverWrapperPlugin;
                    friend class chaos::cu::driver_manager::DriverManager;
					
					bool driverNeedtoDeinitialize;
					
					//! unique uuid for the instance
                    std::string driverUUID;
					
					//! used by driver manager to identity the instance by the hashing
					std::string identificationString;
					
                    boost::atomic_uint accessorCount;
                    
                    //! the list of all generated accessor
                    std::vector<DriverAccessor*> accessors;
                    
                    boost::shared_mutex accessoListShrMux;
                    
                    //! command queue used for receive DrvMsg pack
                    //boost::interprocess::message_queue *commandQueue;
                    DriverQueueType *commandQueue;
					std::auto_ptr<boost::thread> threadMessageReceiver;
					
					
                    // Initialize instance
                    void init(void *initParamPtr) throw(chaos::CException);
                    
                    // Deinit the implementation
                    void deinit() throw(chaos::CException);
					
					
                    //! Wait the new command and broadcast it
                    /*!
					 This method waith for the next command, broadcast it
					 and check if the opcode is the "end of work" opcode,
					 in this case it will quit.
                     */
                    void scanForMessage();
					
                protected:
                    //!Private constructor
                    AbstractDriver();
                    
                    //!Private destructor
                    virtual ~AbstractDriver();

					virtual void driverInit(const char *initParameter) throw(chaos::CException) = 0;
					virtual void driverDeinit()  throw(chaos::CException) = 0;
                public:

                    
                    //! Create a new accessor
                    /*!
                        A new accessor is allocate. In the allocation process
                        the message queue for comunicating with this driver is
                        allocated.
                     */
                    bool getNewAccessor(DriverAccessor **newAccessor);
                    
                    //! Dispose and accessor
                    /*!
                        A driver accessor is relased and all resource are free.
                     */
                    bool releaseAccessor(DriverAccessor *newAccessor);

					
                    //! Execute a command
                    /*!
                        The driver implementation must use the opcode to recognize the
                        command to execute and then write it on th ememory allocate
                        by the issuer of the command.
						\param cmd the message that needs to be executed by the driver implementation
						\return the managment state of the message
                     */
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) = 0;
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractDriver__) */
