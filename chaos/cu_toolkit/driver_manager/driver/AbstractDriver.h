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

#ifndef __CHAOSFramework__AbstractDriver__
#define __CHAOSFramework__AbstractDriver__

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/thread/TemplatedConcurrentQueue.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/BaseBypassDriver.h>
#include <chaos/common/data/CDataWrapper.h>
#include <json/json.h>

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
                public OpcodeExecutor,
				public chaos::common::utility::InizializableService {
                    template<typename T>
                    friend class DriverWrapperPlugin;
                    friend class chaos::cu::driver_manager::DriverManager;
					
					bool driver_need_to_deinitialize;
					
					//! unique uuid for the instance
                    std::string driver_uuid;
					
					//! used by driver manager to identity the instance by the hashing
					std::string identification_string;
					
                    //number of accesso that use this driver instance
                    boost::atomic_uint accessor_count;
                    
                    //! the list of all generated accessor
                    std::vector<DriverAccessor*> accessors;
                    
                    //!accher list shared utex
                    boost::shared_mutex accesso_list_shr_mux;
                    
                    //!decode control unit paramete in json if conversion is applicable
                    bool                            is_json_param;
                    Json::Reader					json_reader;
                    Json::Value						json_parameter_document;
                    
                    //! command queue used for receive DrvMsg pack
                    //boost::interprocess::message_queue *commandQueue;
                    ChaosUniquePtr<DriverQueueType> command_queue;
					ChaosUniquePtr<boost::thread> thread_message_receiver;
					
                    //pointer to the bypassdirver to use
                    ChaosUniquePtr<BaseBypassDriver> bypass_driver;
                    
                    //poit to current executor
                    /*!
                     in default point to the "this" pointer of 
                     this class current instance in case of bypass activated
                     point to the pointer of the bypass class
                     */
                    OpcodeExecutor *o_exe;
                    
                    // Initialize instance
                    void init(void *init_param) throw(chaos::CException);
                    
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
                    AbstractDriver(ChaosUniquePtr<BaseBypassDriver> custom_bypass_driver = ChaosUniquePtr<BaseBypassDriver>(new BaseBypassDriver()));
                    
                    //!Private destructor
                    virtual ~AbstractDriver();

					virtual void driverInit(const char *initParameter) throw(chaos::CException) = 0;
					/*
					 * In case of json initialization driverInit with CDataWrapper is called
					 * */
					virtual void driverInit(const chaos::common::data::CDataWrapper&) throw(chaos::CException);

					virtual void driverDeinit()  throw(chaos::CException) = 0;
                    const bool isDriverParamInJson() const;
                    const bool isBypass()const;
                    /*
                     * called via rpc or via user to implement the bypass
                     * */
                    void setBypass(bool val);

                    const Json::Value& getDriverParamJsonRootElement() const;
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
