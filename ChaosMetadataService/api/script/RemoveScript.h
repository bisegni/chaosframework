/*
 *	RemoveScript.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_B69BCEF7_FAD1_4BC2_9060_940918819817_RemoveScript_h
#define __CHAOSFramework_B69BCEF7_FAD1_4BC2_9060_940918819817_RemoveScript_h

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
                class RemoveScript:
                public AbstractApi {
                    
                public:
                    RemoveScript();
                    ~RemoveScript();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B69BCEF7_FAD1_4BC2_9060_940918819817_RemoveScript_h */
