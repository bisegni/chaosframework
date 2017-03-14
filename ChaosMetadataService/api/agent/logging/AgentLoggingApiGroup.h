/*
 *	AgentLoggingApiGroup.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h
#define __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h

#include "../../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                namespace logging {
                    //! api group for the managment of the logging services
                    DECLARE_CLASS_FACTORY(AgentLoggingApiGroup, AbstractApiGroup) {
                        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(AgentLoggingApiGroup)
                    public:
                        AgentLoggingApiGroup();
                        ~AgentLoggingApiGroup();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h */
