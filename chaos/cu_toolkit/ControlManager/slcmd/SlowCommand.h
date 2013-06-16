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

#include <stdint.h>
#include <string>

namespace chaos{
    namespace cu {
        
        //forward declaration
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }
        
        namespace cm {
            
            //! The name space that group all foundamental class need by slow control !CHOAS implementation
            namespace slcmd {
                
                //forward declaration
                class SlowCommandExecutor;
                
                    //! Namespace for the handler types
                namespace HandlerType {
                    /*!
                     * \enum RunningState
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum Handler {
                        HT_Set              = 1,    /**< Set handler */
                        HT_Acquisition      = 2,    /**< Acquire handler */
                        HT_Crorrelation     = 4     /**< Commit and Correlation handler */
                    } Handler;
                }
                
                    //! Namespace for the running state tyoes
                namespace RunningStateTyoe {
                    /*!
                     * \enum RunningState
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum RunningState {
                        RS_Set      = 1,    /**< The command is the set state */
                        RS_Kill     = 2,    /**< The command can be killed */
                        RS_Over     = 4,    /**< The command can killed by an equal command */
                        RS_Stack    = 8,    /**< The command can be stacked (paused) */
                        RS_Exsc     = 16,   /**< The command cannot be killed or removed, it need to run */
                        RS_End      = 32,   /**< The command has ended his work */
                        RS_Fault    = 64    /**< The command has had a fault */
                    } RunningState;
                }
                
                //! Namespace for the submisison type
                namespace SubmissionRuleType {
                    /*!
                     * \enum SubmissionRule
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum SubmissionRule {
                        SUBMIT_NORMAL       = 0,    /**< The new command will waith the end of the current executed command */
                        SUBMIT_AND_Kill,            /**< The new command will kill the current command */
                        SUBMIT_AND_Overload,        /**< The new command will overload the current command if it is the same*/
                        SUBMIT_AND_Stack            /**< The new command wil stack (pause some or all hadnler) the current executing command */
                    } SubmissionRule;
                }
                
                /*!
                 \struct FaultDescription
                 \brief  Describe the fault of the command. This fileds need to be valorized
                 before to set the rState to RunningState::RS_Fault option
                 */
                typedef struct FaultDescription {
                    uint32_t    code;           /**< The numer code of the error */
                    std::string description;    /**< The description of the fault */
                    std::string domain;         /**< The domain identify the context where the fault is occured */
                } FaultDescription;
                
                
                //! Base cass for the slow command implementation
                /*!
                 The slow command implementation in !CHAOS permit the definition of the three foundamental phase in "control" as seen by !CHAOS logic:
                 - Set Handler, set the start of the slow contro command
                 - Acquire Handler, acquire the data for the command (if needed)
                 - Correlation and Commit handler, make the neccessary correlation and send the necessary command to the driver
                 */
                class SlowCommand {
                    friend class SlowCommandExecutor;
                    //! Running state
                    /*!
                     A value composed by a set of RunningState element.
                     */
                    uint8_t rState;
                    
                    //! Fault description
                    FaultDescription fDescription;
                    
                    typedef uint8_t (SlowCommand::*SetHandlerPtr)();
                    
                    typedef uint8_t (SlowCommand::*AcquireHandlerPtr)();
                    
                    typedef uint8_t (SlowCommand::*CCHandlerPtr)();
                    
                protected:
                    
                    //!The accessor that permit to to contorl the needed driver
                    /*!
                     This is a variable length array where the dimension is given by the driver declared by the CU
                     */
                    dm::driver::DriverAccessor *driverAccessor;
                    
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
                     */
                    virtual uint8_t setHandler();
                    
                    //! Aquire the necessary data for the command
                    /*!
                     The acquire handler has the purpose to get all necessary data need the by CC handler.
                     */
                    virtual uint8_t acquireHandler();
                    
                    //! Correlation and commit phase
                    /*!
                     The correlation and commit handlers has the purpose to check whenever the command has ended or for
                     send further commands to the hardware, through the driver, to accomplish the command steps.
                     */
                    virtual uint8_t ccHandler();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommand__) */
