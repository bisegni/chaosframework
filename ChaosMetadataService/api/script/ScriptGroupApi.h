/*
 *	ScriptGroupApi.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__683DBCD_04CB_4F6F_AF1E_CEC00C352204_ScriptGroupApi_h
#define __CHAOSFramework__683DBCD_04CB_4F6F_AF1E_CEC00C352204_ScriptGroupApi_h


#include "../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                
                //! api group for the managment of the script manageent
                DECLARE_CLASS_FACTORY(ScriptGroupApi, AbstractApiGroup) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ScriptGroupApi)
                public:
                    ScriptGroupApi();
                    ~ScriptGroupApi();
                };
                
            }
        }
    }
}



#endif /* __CHAOSFramework__683DBCD_04CB_4F6F_AF1E_CEC00C352204_ScriptGroupApi_h */