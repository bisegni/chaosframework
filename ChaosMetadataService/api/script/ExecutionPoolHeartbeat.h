/*
 *	ExecutionPoolHeartbeat.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_C1FD7B42_4423_49BA_87AC_A3622D4E4FF6_ExecutionPoolHeartbeat_h
#define __CHAOSFramework_C1FD7B42_4423_49BA_87AC_A3622D4E4FF6_ExecutionPoolHeartbeat_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Add a new instance to the script
                class ExecutionPoolHeartbeat:
                public AbstractApi {
                    
                public:
                    ExecutionPoolHeartbeat();
                    ~ExecutionPoolHeartbeat();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C1FD7B42_4423_49BA_87AC_A3622D4E4FF6_ExecutionPoolHeartbeat_h */
