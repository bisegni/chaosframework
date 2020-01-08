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
                    
                    //!specify when the infrastructure need to manage busy flag
                    bool auto_busy;
                    
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
                    void setStateVariableSeverity(StateVariableType variable_type,
                                                  const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                    
                    //!set the alarm state
                    bool setStateVariableSeverity(StateVariableType variable_type,
                                                  const std::string& state_variable__name,
                                                  const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                    //!set the alarm state_variable
                    bool setStateVariableSeverity(StateVariableType variable_type,
                                                  const unsigned int state_variable__ordered_id,
                                                  const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                    //!get the current alarm state
                    bool getStateVariableSeverity(StateVariableType variable_type,
                                                  const std::string& state_variable__name,
                                                  chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                    //!get the current alarm state
                    bool getStateVariableSeverity(StateVariableType variable_type,
                                                  const unsigned int state_variable_ordered_id,
                                                  chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                    //!set the busy flag
                    void setBusyFlag(bool state);
                    
                    //!get the busy flag
                    bool getBusyFlag();
                    
                    void metadataLogging(const chaos::common::metadata_logging::StandardLoggingChannel::LogLevel log_level,
                                         const std::string& message);
                    //!inherited methods
                    void startHandler();
                    void endHandler();
                public:
                    
                    //! return the identification of the device
                    const string & getDeviceID();
                    
                     //! return the load parameters of the device
                    const string getDeviceLoadParams();

                    //!  return the load parameters of the device
                    //! @param parm 
                    //! return 0 if success
                    int getDeviceLoadParams(chaos::common::data::CDataWrapper&parm);
                    //! se the auto busy behaviour
                    /*!
                     Auto busy, let the control unit to manage the busy flag in automatic mode
                     for the instance of command the set it to true;
                     */
                    void setAutoBusy(bool new_auto_busy);
                    
                    //! return the auto busy flag
                    const bool isAutoBusy();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommand__) */
