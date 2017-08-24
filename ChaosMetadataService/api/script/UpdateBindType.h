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
                    int updateBindType(const chaos::service_common::data::script::ScriptBaseDescription& script_base_descrition,
                                       const chaos::service_common::data::script::ScriptInstance& instance);
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
