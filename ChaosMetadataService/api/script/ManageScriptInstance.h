/*
 *	ManageScriptInstance.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_E7B0A98B_A6AB_4230_9822_34686FA58C52_ManageScriptInstance_h
#define __CHAOSFramework_E7B0A98B_A6AB_4230_9822_34686FA58C52_ManageScriptInstance_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Add a new instance to the script
                class ManageScriptInstance:
                public AbstractApi {
                    
                public:
                    ManageScriptInstance();
                    ~ManageScriptInstance();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}


#endif /* __CHAOSFramework_E7B0A98B_A6AB_4230_9822_34686FA58C52_ManageScriptInstance_h */
