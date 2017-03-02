/*
 *	DeployApiGroup.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h
#define __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h

#include "../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace deploy {
                
                //! api group for the managment of the logging services
                DECLARE_CLASS_FACTORY(DeployApiGroup, AbstractApiGroup) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DeployApiGroup)
                public:
                    DeployApiGroup();
                    ~DeployApiGroup();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h */
