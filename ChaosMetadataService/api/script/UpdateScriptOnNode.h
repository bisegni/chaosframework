/*
 *	UpdateScriptOnNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__BE23F01_12D4_457B_ADD9_21D2C0DAB6A5_UpdateScriptOnNode_h
#define __CHAOSFramework__BE23F01_12D4_457B_ADD9_21D2C0DAB6A5_UpdateScriptOnNode_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Update the script on a determinate node or series of node
                class UpdateScriptOnNode:
                public AbstractApi {
                    
                public:
                    UpdateScriptOnNode();
                    ~UpdateScriptOnNode();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__BE23F01_12D4_457B_ADD9_21D2C0DAB6A5_UpdateScriptOnNode_h */