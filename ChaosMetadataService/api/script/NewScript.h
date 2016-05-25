/*
 *	NewScript.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_NewScript_h
#define __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_NewScript_h

#include "../AbstractApi.h"

#include <chaos_service_common/data/script/Script.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Create a new script element in persistent database
                /*!
                 Perform the creation of new script with all data passed in input
                 */
                class NewScript:
                public AbstractApi {

                public:
                    NewScript();
                    ~NewScript();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_NewScript_h */
