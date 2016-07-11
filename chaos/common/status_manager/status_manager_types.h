/*
 *	status_manager_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h
#define __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h

#include <string>

namespace chaos {
    namespace common {
        namespace status {
            
            typedef enum {
                StatusFlagServerityNormalOperationl,
                StatusFlagServerityWarning,
                StatusFlagServerityBlocking
            }StatusFlagServerity;
            
            //! Status Flag description
            struct StatusFlag {
                std::string name;
                std::stirng description;
                StatusFlagServerity severity;

            };
            
        }
    }
}

#endif /* __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h */
