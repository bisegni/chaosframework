/*
 *	GetDomains.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__GetDomains_h
#define __CHAOSFramework__GetDomains_h

#include "../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace groups {
                //! Return all child of a node
                /*!
                 
                 */
                class GetDomains:
                public AbstractApi {
                public:
                    GetDomains();
                    ~GetDomains();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetDomains_h */
