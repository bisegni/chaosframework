/*
 *	BatchCommand.h
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

#ifndef __CHAOSFramework__BatchCommand__
#define __CHAOSFramework__BatchCommand__

#include <bitset>
#include <vector>
#include <string>
#include <stdint.h>

#include <boost/chrono.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>

#include <chaos/common/batch_command/BatchCommandTypes.h>
#include <chaos/common/batch_command/BatchCommandDescription.h>

using namespace chaos::common::data;
using namespace chaos::common::data::cache;

namespace chaos{
    
    namespace common {
        
        //! The name space that group all foundamental class need by slow control !CHAOS implementation
        namespace batch_command {
            class AbstractSandbox;
            //forward declaration
            //class BatchCommandExecutor;
            
            //!help macro for set the sate
#define BC_EXCLUSIVE_RUNNING_PROPERTY    setRunningProperty(chaos::common::batch_command::RunningPropertyType::RP_EXSC);
#define BC_NORMAL_RUNNING_PROPERTY  setRunningProperty(chaos::common::batch_command::RunningPropertyType::RP_NORMAL);
#define BC_END_RUNNING_PROPERTY     setRunningProperty(chaos::common::batch_command::RunningPropertyType::RP_END);
#define BC_FAULT_RUNNING_PROPERTY   setRunningProperty(chaos::common::batch_command::RunningPropertyType::RP_FAULT);
#define BC_FATAL_FAULT_RUNNING_PROPERTY   setRunningProperty(chaos::common::batch_command::RunningPropertyType::RP_FATAL_FAULT);

            //help madro to get the state
#define BC_CHECK_EXEC_RUNNING_PROPERTY  (getRunningProperty() == chaos::common::batch_command::RunningPropertyType::RP_EXSC)
#define BC_CHECK_NORMAL_RUNNING_PROPERTY (getRunningProperty() == chaos::common::batch_command::RunningPropertyType::RP_NORMAL)
#define BC_CHECK_END_RUNNING_PROPERTY   (getRunningProperty() == chaos::common::batch_command::RunningPropertyType::RP_END)
#define BC_CHECK_FAULT_RUNNING_PROPERTY (getRunningProperty() == chaos::common::batch_command::RunningPropertyType::RP_FAULT)
#define BC_CHECK_FATAL_FAULT_RUNNING_PROPERTY (getRunningProperty() == chaos::common::batch_command::RunningPropertyType::RP_FATAL_FAULT)

            
            //! Collect the command timing stats
            typedef struct CommandTimingStats {
                //! command start microsecond (sandbox time)
                uint64_t command_set_time_usec;
                
                //! count the step phases
                uint64_t command_step_counter;
            } CommandTimingStats;
            
            //! Base cass for the slow command implementation
            /*!
             The slow command implementation in !CHAOS permit the definition of the three foundamental phase in "control" as seen by !CHAOS logic:
             - Set Handler, set the start of the slow contro command
             - Acquire Handler, acquire the data for the command (if needed)
             - Correlation and Commit handler, make the neccessary correlation and send the necessary command to the driver
             */
            class BatchCommand {
                friend class BatchCommandSandbox;
                friend class BatchCommandExecutor;
                friend class AbstractSandbox;
                friend struct AcquireFunctor;
                friend struct CorrelationFunctor;
                friend struct CommandInfoAndImplementation;
                bool sticky;
                //!unique command id
                uint64_t unique_id;
                
                //! the alias associated to the command
                std::string command_alias;
                
                //! keep track whenever set handler has been called
                bool already_setupped;
                
                //!command timing stat
                CommandTimingStats timing_stats;
                
                //! Locking flag
                /*!
                 this set keep track of the lock state for the feature
                 or the running property for the mdification applyed by
                 subclass developer lock=1/unlock=0
                 first bit is for features second bit is for runnign property
                 third bit is used by sandobox to lock all feature in th emiddle of the change of the command
                 */
                std::bitset<3> lockFeaturePropertyFlag;
                
                //! Command features
                features::Features commandFeatures;
                
                //!Shared sandbox stat
                const SandboxStat *shared_stat;
                
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
                
                //! Fault description
                FaultDescription fault_description;
                
                //! shared setting across all slow command
                AbstractSharedDomainCache *sharedAttributeCachePtr;
                
                //! called befor the command start the execution
                void commandPre();
                
                //! called after the command step excecution
                void commandPost();
                
            protected:
                
                //! default constructor
                BatchCommand();
                
                //! default destructor
                virtual ~BatchCommand();
                
                virtual
                AbstractSharedDomainCache * const getSharedCacheInterface();
                
            public:
                //! return the unique id for the command instance
                uint64_t getUID();
                /**
                 * Return the alias of the command
                 * @return the alis name of the command
                 */
                std::string getAlias(){return command_alias;}
                //! return the identification of the device
                std::string& getDeviceID();
                
                //! return the set handler time in milliseocnds
                uint64_t getSetTime();
                
                //! return the start step time of the sandbox in milliseocnds
                uint64_t getStartStepTime();
                
                uint64_t getStepCounter();
                
                //! return the last step duration of the sandbox in microseconds
                uint64_t getLastStepDuration();
                
                //! set the alias of the command
                void setCommandAlias(const std::string& _command_alias);
                
                //! set the features with the uint32 value
                /*!
                 Feature rappresented by an uint32 can be setupped with this api. The value can be
                 overloaded by submition feature flag and anyway the are keept in consideration onlyat installation time
                 of the command.
                 */
                inline void setFeatures(features::FeaturesFlagTypes::FeatureFlag features, uint32_t features_value) {
                    //check if the features are locked for the user modifications
                    if(lockFeaturePropertyFlag.test(0) || lockFeaturePropertyFlag.test(2)) return;
                    
                    
                    switch (features) {
                        case features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY:
                            commandFeatures.featuresFlag |= features;
                            commandFeatures.featureSchedulerStepsDelay = features_value;
                            break;
                            
                        case features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT:
                            commandFeatures.featuresFlag |= features;
                            commandFeatures.featureCommandTimeout = features_value;
                            break;
                            
                        case features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY:
                            commandFeatures.featuresFlag |= features;
                            commandFeatures.featureSubmissionRetryDelay = features_value;
                            break;
                            
                        default:
                            break;
                    }
                }
                
                //! set the features with the uint32 value
                /*!
                 Feature rappresented by an uint32 can be setupped with this api. The value can be
                 overloaded by submition feature flag and anyway the are keept in consideration onlyat installation time
                 of the command.
                 */
                inline void setFeatures(features::FeaturesFlagTypes::FeatureFlag features, uint64_t features_value) {
                    //check if the features are locked for the user modifications
                    if(lockFeaturePropertyFlag.test(0) || lockFeaturePropertyFlag.test(2)) return;
                    
                    
                    switch (features) {
                        case features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY:
                            commandFeatures.featuresFlag |= features;
                            commandFeatures.featureSchedulerStepsDelay = features_value;
                            break;
                            
                        case features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT:
                            commandFeatures.featuresFlag |= features;
                            commandFeatures.featureCommandTimeout = features_value;
                        default:
                            break;
                    }
                }
                
                //! clear the features
                /*!
                 Turn off the features
                 \param features is the features that need to be cleared
                 */
                inline void clearFeatures(features::FeaturesFlagTypes::FeatureFlag features) {
                    //check if the features are locked for the user modifications
                    if(lockFeaturePropertyFlag.test(0) || lockFeaturePropertyFlag.test(2)) return;
                    
                    commandFeatures.featuresFlag ^= features;
                    switch (features) {
                        case features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY:
                            commandFeatures.featureSchedulerStepsDelay = 0;
                            break;
                            
                        case features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY:
                            commandFeatures.featureSubmissionRetryDelay = 0;
                            break;
                            
                        case features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT:
                            commandFeatures.featureCommandTimeout = 0;
                        default:
                            break;
                    }
                }
                
                //! set the running property
                virtual bool setRunningProperty(uint8_t property);
                
                //! return the current running property
                virtual uint8_t getRunningProperty();
                
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
                virtual void setHandler(chaos::common::data::CDataWrapper *data);
                
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
                
                //! Handle the timeout
                /*!
                 Permit to the command implementation to handle the timeout, timeout continue to be called untile command quit after timeout is reached
                 \return true if a fault error, for the timeout, need to be set.
                 */
                virtual bool timeoutHandler();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommand__) */
