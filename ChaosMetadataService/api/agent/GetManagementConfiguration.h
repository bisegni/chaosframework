/*
 *	GetManagementConfiguration.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/05/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_C5361E0A_631E_4EA2_A3DD_A982FA1A0366_GetManagementConfiguration_h
#define __CHAOSFramework_C5361E0A_631E_4EA2_A3DD_A982FA1A0366_GetManagementConfiguration_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! Return all mds management configuration for agent
                /*!
                 
                 */
                class GetManagementConfiguration:
                public AbstractApi {
                    
                public:
                    GetManagementConfiguration();
                    ~GetManagementConfiguration();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C5361E0A_631E_4EA2_A3DD_A982FA1A0366_GetManagementConfiguration_h */
