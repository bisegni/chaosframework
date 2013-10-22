/*
 *	SlowCommandTypes.h
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

#ifndef CHAOSFramework_SlowCommandTypes_h
#define CHAOSFramework_SlowCommandTypes_h

#include <string>

#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

namespace chaos_data = chaos::common::data;

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                class SlowCommand;
                
                typedef  uint8_t BitBlockDimension;
                typedef  uint16_t AttributeIndexType;
                
                /*!
                 Type used for the next available command impl and description
                 into the sandbox
                 */
                typedef struct {
                    chaos::PRIORITY_ELEMENT(chaos_data::CDataWrapper) *cmdInfo;
                    SlowCommand *cmdImpl;
                } CommandInfoAndImplementation;
                
				
                //! Namespace for the handler types
                namespace HandlerType {
                    /*!
                     * \enum RunningState
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum Handler {
                        HT_Set              = 1,    /**< Set handler */
                        HT_Acquisition      = 2,    /**< Acquire handler */
                        HT_Correlation      = 4     /**< Commit and Correlation handler */
                    } Handler;
                }
                
                //! Namespace for the running state tyoes
                namespace RunningStateType {
                    /*!
                     * \enum RunningState
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum RunningState {
                        RS_Exsc     = 0,    /**< The command cannot be killed or removed, it need to run */
                        RS_Stack    = 1,    /**< The command can be stacked (paused) */
                        RS_Kill     = 2,    /**< The command can be killed */
                        RS_End      = 4,    /**< The command has ended his work */
                        RS_Fault    = 8    /**< The command has had a fault */
                    } RunningState;
                }

			}
        }
    }
}
#endif
