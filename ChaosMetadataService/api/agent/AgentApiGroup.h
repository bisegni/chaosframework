/*
 *	AgentApiGroup.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__58410B3_3E94_4B8E_9EFE_3ECFC650FBA9_AgentApiGroup_h
#define __CHAOSFramework__58410B3_3E94_4B8E_9EFE_3ECFC650FBA9_AgentApiGroup_h

#include "../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! api group for the managment of the logging services
                DECLARE_CLASS_FACTORY(AgentApiGroup, AbstractApiGroup) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(AgentApiGroup)
                public:
                    AgentApiGroup();
                    ~AgentApiGroup();
                };
                
            }
        }
    }
}


#endif /* __CHAOSFramework__58410B3_3E94_4B8E_9EFE_3ECFC650FBA9_AgentApiGroup_h */
