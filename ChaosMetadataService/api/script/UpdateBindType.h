/*
 *	UpdateBindType.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 23/05/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__6B052A4_BE26_4241_B2A6_6E43ECE5D031_UpdateBindType_h
#define __CHAOSFramework__6B052A4_BE26_4241_B2A6_6E43ECE5D031_UpdateBindType_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Update the script on a determinate node or series of node
                class UpdateBindType:
                public AbstractApi {
                    
                public:
                    UpdateBindType();
                    ~UpdateBindType();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__6B052A4_BE26_4241_B2A6_6E43ECE5D031_UpdateBindType_h */
