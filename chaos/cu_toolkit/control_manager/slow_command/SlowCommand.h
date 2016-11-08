/*
 *	SlowCommand.h
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

#ifndef __CHAOSFramework__SlowCommand__
#define __CHAOSFramework__SlowCommand__

#include <bitset>
#include <vector>
#include <string>
#include <stdint.h>

#include <boost/chrono.hpp>

#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/batch_command/BatchCommand.h>

#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>
#include <chaos/cu_toolkit/control_manager/AttributeSharedCacheWrapper.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

namespace chaos_data = chaos::common::data;

namespace chaos{
    namespace cu {
        
        //forward declarations
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }
        
        namespace control_manager {
            
            //! The name space that group all foundamental class need by slow control !CHAOS implementation
            namespace slow_command {
                
                namespace command {
                    class SetAttributeCommand;
                }
                
                //forward declaration
                class SlowCommandExecutor;
                
                //! Macro for helping the allocation of the isntancer of the class implementing the slow command
                
                /*
                 * You may specify command name
                 */
#define BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(n,c,alias, d, uid) \
BATCH_COMMAND_OPEN_DESC(n,c, alias, d, uid) \
result->setInstancer(new chaos::common::utility::NestedObjectInstancer<chaos::cu::control_manager::slow_command::SlowCommand, chaos::common::batch_command::BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<n c, chaos::cu::control_manager::slow_command::SlowCommand>()));
                
                /**
                 * Command name is the class name
                 */
#define BATCH_COMMAND_OPEN_DESCRIPTION(n,c, d, uid) BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(n,c, # c, d, uid)
                
                
                
                //! Base cass for the slow command implementation
                /*!
                 The slow command implementation in !CHAOS permit the definition of the three foundamental phase in "control" as seen by !CHAOS logic:
                 - Set Handler, set the start of the slow contro command
                 - Acquire Handler, acquire the data for the command (if needed)
                 - Correlation and Commit handler, make the neccessary correlation and send the necessary command to the driver
                 */
                class SlowCommand:
                public chaos::common::batch_command::BatchCommand {
                    friend class SlowCommandExecutor;
                    friend class command::SetAttributeCommand;
                    
                    //! point to the in memory device database
                    chaos::cu::control_manager::AbstractControlUnit  *abstract_control_unit;
                    
                    //redefine the visibility
                    AbstractSharedDomainCache * const getSharedCacheInterface() {
                        return chaos::common::batch_command::BatchCommand::getSharedCacheInterface();
                    }
                    
                    //! shared attribute cache
                    AttributeSharedCacheWrapper * attribute_cache;
                protected:
                    
                    //! The erogator of the driver requested by the control unit
                    chaos::cu::driver_manager::DriverErogatorInterface *driverAccessorsErogator;
                    
                    //! default constructor
                    SlowCommand();
                    
                    //! default destructor
                    virtual ~SlowCommand();
                    
                    /*!
                     return the device database with the dafualt device information
                     */
                    chaos_data::DatasetDB  * const getDeviceDatabase();
                    
                    
                    //! return the attribute cache pointer
                    AttributeSharedCacheWrapper * const getAttributeCache();
                    
                    //---------------alarm api-------------
                    //!set the severity on all alarm
                    void setAlarmSeverity(const common::alarm::MultiSeverityAlarmLevel alarm_severity);
                    
                    //!set the alarm state
                    bool setAlarmSeverity(const std::string& alarm_name,
                                          const chaos::common::alarm::MultiSeverityAlarmLevel state_code);
                    //!set the alarm state
                    bool setAlarmSeverity(const unsigned int alarm_ordered_id,
                                          const chaos::common::alarm::MultiSeverityAlarmLevel state_code);
                    //!get the current alarm state
                    bool getAlarmSeverity(const std::string& alarm_name,
                                          chaos::common::alarm::MultiSeverityAlarmLevel& current_state);
                    //!get the current alarm state
                    bool getAlarmSeverity(const unsigned int alarm_ordered_id,
                                          chaos::common::alarm::MultiSeverityAlarmLevel& current_state);
                    //!set the busy flag
                    void setBusyFlag(bool state);
                    
                    //!get the busy flag
                    bool getBusyFlag();
                public:
                    
                    //! return the identification of the device
                    const string & getDeviceID();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommand__) */
