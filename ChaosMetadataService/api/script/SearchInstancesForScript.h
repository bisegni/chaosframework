/*
 *	SearchInstancesForScript.h
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

#ifndef __CHAOSFramework_A2CDE7DC_50DD_4722_9425_C6335E1304F9_SearchInstancesForScript_h
#define __CHAOSFramework_A2CDE7DC_50DD_4722_9425_C6335E1304F9_SearchInstancesForScript_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Load full script
                //! the only information needed are the sequence and name
                class SearchInstancesForScript:
                public AbstractApi {
                    
                public:
                    SearchInstancesForScript();
                    ~SearchInstancesForScript();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_A2CDE7DC_50DD_4722_9425_C6335E1304F9_SearchInstancesForScript_h */
