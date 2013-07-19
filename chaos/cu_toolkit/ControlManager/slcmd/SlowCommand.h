/*
 *	SlowCommand.h
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

#ifndef __CHAOSFramework__SlowCommand__
#define __CHAOSFramework__SlowCommand__

#include <map>
#include <vector>
#include <string>
#include <stdint.h>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/ChannelSetting.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandTypes.h>
namespace chaos{
    namespace cu {
        
        //forward declarations
        class AbstractedControlUnit;
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }
        
        namespace control_manager {
            
            //! The name space that group all foundamental class need by slow control !CHOAS implementation
            namespace slow_command {
                
                //forward declaration
                class SlowCommandExecutor;
                
#define SL_EXEC_RUNNIG_STATE chaos::cu::control_manager::slow_command::SlowCommand::runningState = chaos::cu::control_manager::slow_command::RunningStateType::RS_Exsc;
#define SL_STACK_RUNNIG_STATE chaos::cu::control_manager::slow_command::SlowCommand::runningState = chaos::cu::control_manager::slow_command::RunningStateType::RS_Stack;
#define SL_KILL_RUNNIG_STATE chaos::cu::control_manager::slow_command::SlowCommand::runningState = chaos::cu::control_manager::slow_command::RunningStateType::RS_Kill;
#define SL_END_RUNNIG_STATE chaos::cu::control_manager::slow_command::SlowCommand::runningState = chaos::cu::control_manager::slow_command::RunningStateType::RS_End;
#define SL_FAULT_RUNNIG_STATE chaos::cu::control_manager::slow_command::SlowCommand::runningState = chaos::cu::control_manager::slow_command::RunningStateType::RS_Fault;
                
                //! Base cass for the slow command implementation
                /*!
                 The slow command implementation in !CHAOS permit the definition of the three foundamental phase in "control" as seen by !CHAOS logic:
                 - Set Handler, set the start of the slow contro command
                 - Acquire Handler, acquire the data for the command (if needed)
                 - Correlation and Commit handler, make the neccessary correlation and send the necessary command to the driver
                 */
                class SlowCommand {
                    friend class SlowCommandSandbox;
                    friend class SlowCommandExecutor;
                    friend struct SetFunctor;
                    friend struct AcquireFunctor;
                    friend struct CorrelationFunctor;
                    
                    //! Submission state
                    /*!
                     A value that represent the submiossion state
                     */
                    uint8_t submissionRule;
                    
                    KeyDataStorage *keyDataStorage;
                    
                    //! Fault description
                    FaultDescription fDescription;
                    
                    //! shared setting across all slow command
                    ChannelSetting *sharedChannelSettingPtr;
                    
                    //!Set Handlers definition
                    typedef uint8_t (SlowCommand::*HandlerPointer)();
                    
                    //! Map that assocaite the alias to the driver accessor
                    /*!
                     The map is only a pointer becaouse someone else need to set it.
                     The subclass ca access it for get the accessor.
                     */
                    std::map<std::string, dm::driver::DriverAccessor*> *driversAccessorMap;
                    
                protected:
                    //! Running state
                    /*!
                     A value composed by a set of RunningState element.
                     */
                    uint8_t runningState;
                    
                    //! default constructor
                    SlowCommand();
                    
                    //! default destructor
                    virtual ~SlowCommand();
                    
                    /*
                     Send device data to output buffer
                     */
                    void pushDataSet(chaos::CDataWrapper *acquiredData);
                    
                    /*
                     Return a new instance of CDataWrapper filled with a mandatory data
                     according to key
                     */
                    chaos::CDataWrapper *getNewDataWrapper();
                    
                    //! return the implemented handler
                    /*!
                     The implementation need to give information about the handler
                     that has been implemented. If all handler has been implemented
                     the result will be "HT_Set | HT_Acq | HT_CrCm"
                     */
                    virtual uint8_t implementedHandler() = 0;
                    
                    //! Start the command execution
                    /*!
                     Set handler has the main purpose to initiate the command. All the operration need to avviate
                     the command sequence need to made here.
                     \param data CDatawrapper object taht containing a set of initial data for the command
                     \return the mask for the runnign state
                     */
                    virtual void setHandler(chaos::CDataWrapper *data);
                    
                    //! Aquire the necessary data for the command
                    /*!
                     The acquire handler has the purpose to get all necessary data need the by CC handler.
                     \return the mask for the runnign state
                     */
                    virtual void acquireHandler();
                    
                    //! Correlation and commit phase
                    /*!
                     The correlation and commit handlers has the purpose to check whenever the command has ended or for
                     send further commands to the hardware, through the driver, to accomplish the command steps.
                     \return the mask for the runnign state
                     */
                    virtual void ccHandler();
                    
                public:

                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommand__) */
