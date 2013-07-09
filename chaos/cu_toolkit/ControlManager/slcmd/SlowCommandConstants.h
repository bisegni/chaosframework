/*
 *	SlowCommandConstants.h
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

#ifndef CHAOSFramework_SlowCommandConstants_h
#define CHAOSFramework_SlowCommandConstants_h
namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                
                /** @defgroup SlowCommandSubmissionKey Chaos slow control submission key
                 @{
                 */
                //! Name space for grupping option used for regulate the submission of the slow control
                namespace SlowCommandSubmissionKey {
                    //! @The key represent the alis of the comand to instantiate, the value is a string
                    static const char * const   COMMAND_ALIAS     = "command_alias";
                    //! @The key represent the option to the command submission, the value is an uint32
                    static const char * const   SUBMISSION_RULE     = "submission_rule";
                    //! @The key represent the option to the command submission priority, the value is an uint32
                    static const char * const   SUBMISSION_PRIORITY     = "submission_priority";
                }
                /** @} */ // end of SlowCommandSubmissionKey
            }
        }
    }
}

#endif
