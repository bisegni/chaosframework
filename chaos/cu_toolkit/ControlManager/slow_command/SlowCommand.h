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

#include <bitset>
#include <vector>
#include <string>
#include <stdint.h>

#include <boost/chrono.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/DatasetDB.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/AttributeSetting.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandTypes.h>

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
            
            //! The name space that group all foundamental class need by slow control !CHOAS implementation
            namespace slow_command {
                
                //forward declaration
                class SlowCommandExecutor;
                
#define SL_EXEC_RUNNIG_STATE    setRunningProperty(chaos::cu::control_manager::slow_command::RunningStateType::RS_Exsc);
#define SL_STACK_RUNNIG_STATE   setRunningProperty(chaos::cu::control_manager::slow_command::RunningStateType::RS_Stack);
#define SL_KILL_RUNNIG_STATE    setRunningProperty(chaos::cu::control_manager::slow_command::RunningStateType::RS_Kill);
#define SL_END_RUNNIG_STATE     setRunningProperty(chaos::cu::control_manager::slow_command::RunningStateType::RS_End);
#define SL_FAULT_RUNNIG_STATE   setRunningProperty(chaos::cu::control_manager::slow_command::RunningStateType::RS_Fault);
                
#define SL_CHECK_EXEC_RUNNIG_STATE  (getRunningProperty() & chaos::cu::control_manager::slow_command::RunningStateType::RS_Exsc)
#define SL_CHECK_STACK_RUNNIG_STATE (getRunningProperty() & chaos::cu::control_manager::slow_command::RunningStateType::RS_Stack)
#define SL_CHECK_KILL_RUNNIG_STATE  (getRunningProperty() & chaos::cu::control_manager::slow_command::RunningStateType::RS_Kill)
#define SL_CHECK_END_RUNNIG_STATE   (getRunningProperty() & chaos::cu::control_manager::slow_command::RunningStateType::RS_End)
#define SL_CHECK_FAULT_RUNNIG_STATE (getRunningProperty() & chaos::cu::control_manager::slow_command::RunningStateType::RS_Fault)
                
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
                    friend struct AcquireFunctor;
                    friend struct CorrelationFunctor;
					
					uint64_t unique_id;
					
					//! Locking flag
					/*!
						this set keep track of the lock state for the feature
						or the running property for the mdification applyed by 
						subclass developer lock=1/unlock=0
						first bit is for features second bit is for runnign property
					 */
					std::bitset<2> lockFeaturePropertyFlag;

					
					//! Command features
					features::Features commandFeatures;
					
                    //! Running state
                    /*!
                     A value composed by a set of RunningState element.
                     */
                    uint8_t runningProperty;
					
                    //! Submission state
                    /*!
                     A value that represent the submiossion state
                     */
                    uint8_t submissionRule;
                    
                    //! key data storage to forwsard data to central memory (momentary until directi/O will be created)
                    KeyDataStorage *keyDataStoragePtr;
                    
                    //! point to the in memory device database
					chaos::common::data::DatasetDB  *deviceDatabasePtr;
                    
                    //! Fault description
                    FaultDescription faultDescription;
                    
                    //! shared setting across all slow command
                    IOCAttributeShareCache *sharedAttributeSettingPtr;
                    
                protected:
					//! The erogator of the driver requested by the control unit
					chaos::cu::driver_manager::DriverErogatorInterface *driverAccessorsErogator;
					
                    //Shared sandbox stat
                    const SandboxStat *shared_stat;

                    
                    //! default constructor
                    SlowCommand();
                    
                    //! default destructor
                    virtual ~SlowCommand();
                    
                    //! set the features with the uint32 value
                    /*!
                     Feature rappresented by an uint32 can be setupped with this api. The value can be 
                     overloaded by submition feature flag and anyway the are keept in consideration onlyat installation time
                     of the command.
                     */
					inline void setFeatures(features::FeaturesFlagTypes::FeatureFlag features, uint32_t features_value) {
						//check if the features are locked for the user modifications
						if(lockFeaturePropertyFlag.test(0)) return;
						
						commandFeatures.featuresFlag |= features;
						switch (features) {
							case features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY:
								commandFeatures.featureSchedulerStepsDelay = features_value;
								break;
								
							case features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY:
								commandFeatures.featureSubmissionRetryDelay = features_value;
								break;
								
							default:
								break;
						}
					}
                    
					//! set the running property
					inline void setRunningProperty(uint8_t property)  {
						if(lockFeaturePropertyFlag.test(1)) return;
						runningProperty = property;
					}
					
					//! return the current running property
					inline uint8_t getRunningProperty() {
						return runningProperty;
					}
					
                    /*!
                     return the device database with the dafualt device information
                     */
                    chaos_data::DatasetDB  *getDeviceDatabase();
                    
					//! Return the value object for the domain and the string key
					/*!
						\param domain a domain identified by a value of @IOCAttributeShareCache::SharedVeriableDomain
						\key_name a name that identify the variable
					 */
                    ValueSetting *getVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, const char *variable_name);
					
					//! Return the value object for the domain and the index of the variable
                    ValueSetting *getVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, VariableIndexType variable_index);
                    
					//! Set the value for a determinated variable in a determinate domain
                    void setVariableValueForKey(IOCAttributeShareCache::SharedVeriableDomain domain, const char *variable_name, void * value, uint32_t size);
					
					//! Get the index of the changed attribute
                    void getChangedVariableIndex(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<VariableIndexType>& changed_index);
                    
					//! Return the names of all variabl einto a determinated domain
                    void getVariableNames(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<std::string>& names);
                    
                    /*
                     Send device data to output buffer
                     */
                    void pushDataSet(chaos_data::CDataWrapper *acquired_data);
                    
                    /*
                     Return a new instance of CDataWrapper filled with a mandatory data
                     according to key
                     */
                    chaos_data::CDataWrapper *getNewDataWrapper();
                    
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
                    virtual void setHandler(chaos_data::CDataWrapper *data);
                    
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
