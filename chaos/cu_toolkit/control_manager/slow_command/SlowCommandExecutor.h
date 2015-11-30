/*
 *	SlowCommandExecutor.h
 *	!CHAOS
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


#ifndef __CHAOSFramework__SlowCommandExecutor__
#define __CHAOSFramework__SlowCommandExecutor__

#include <map>
#include <deque>
#include <memory>
#include <stdint.h>


#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/atomic.hpp>

#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>
#include <chaos/common/utility/ObjectInstancer.h>

#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>


namespace chaos {
    namespace cu {
        
        //forward declaration
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }
        
        namespace control_manager {
			//forward declaration
			class AbstractControlUnit;
			class SCAbstractControlUnit;

            namespace slow_command {
				
				namespace boost_cont = boost::container;
				namespace chaos_data = chaos::common::data;
				namespace chaos_cache = chaos::common::data::cache;
				namespace chaos_batch = chaos::common::batch_command;
				
                //forward declaration
                class SlowCommand;
                
#define SLOWCOMMAND_INSTANCER(SlowCommandClass)\
new chaos::common::utility::NestedObjectInstancer<chaos::cu::control_manager::slow_command::SlowCommand, chaos::common::batch_command::BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<SlowCommandClass, chaos::cu::control_manager::slow_command::SlowCommand>())
                
                //! Slow command execution sand box
                /*!
                    This class is the environment where the exeecution of the slow command handlers take place.
                 */
                class SlowCommandExecutor : private chaos_batch::BatchCommandExecutor {

                    friend class chaos::cu::control_manager::SCAbstractControlUnit;
                    
                    //! the reference to the master device database
                    chaos_data::DatasetDB *dataset_attribute_db_ptr;
                    
					//! The driver erogator
					chaos::cu::driver_manager::DriverErogatorInterface *driverAccessorsErogator;
					
					//! attribute cache wrapper
					AttributeSharedCacheWrapper * attribute_cache;
					
					AbstractControlUnit *control_unit_instance;
					//fast unit last id cache value
					AttributeValue *last_ru_id_cache;
					//fast cache error variable accessor
					AttributeValue *last_error_code;
					AttributeValue *last_error_message;
					AttributeValue *last_error_domain;
                protected:
                    
                    //! Private constructor
                    SlowCommandExecutor(const std::string& _executorID,
										chaos_data::DatasetDB *_deviceSchemaDbPtr,
										SCAbstractControlUnit *_control_unit_instance);
                    
                    //! Private deconstructor
                    ~SlowCommandExecutor();
                    
                    //allocate a new command
                    chaos_batch::BatchCommand *instanceCommandInfo(const std::string& command_alias,
                                                                   chaos::common::data::CDataWrapper *command_info);
					
					//overlodaed command event handler
					void handleCommandEvent(uint64_t command_seq,
											chaos_batch::BatchCommandEventType::BatchCommandEventType type,
											void* type_value_ptr,
											uint32_t type_value_size);
					
					//! general sandbox event handler
					void handleSandboxEvent(const std::string& sandbox_id,
											chaos_batch::BatchSandboxEventType::BatchSandboxEventType type,
											void* type_value_ptr,
											uint32_t type_value_size);
                public:
                    
                    // Initialize instance
                    void init(void*) throw(chaos::CException);

					// Start the implementation
					void start() throw(chaos::CException);
					
					// Start the implementation
					void stop() throw(chaos::CException);
					
					// Deinit instance
					void deinit() throw(chaos::CException);

					//! Install a command associated with a type
                    void installCommand(const std::string& alias,
										chaos::common::utility::NestedObjectInstancer<SlowCommand, chaos_batch::BatchCommand> *instancer);
                    //! Install a command
                    void installCommand(boost::shared_ptr<common::batch_command::BatchCommandDescription> command_description);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandExecutor__) */
