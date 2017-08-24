/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
