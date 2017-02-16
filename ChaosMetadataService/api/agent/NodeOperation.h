/*
 *	NodeOperation.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 13/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EFEDD110_E100_4081_8C2E_8D22E3C5C5E8_NodeOperation_h
#define __CHAOSFramework_EFEDD110_E100_4081_8C2E_8D22E3C5C5E8_NodeOperation_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! remove an association from an angen and a node
                /*!
                 
                 */
                class NodeOperation:
                public AbstractApi {
                    
                public:
                    NodeOperation();
                    ~NodeOperation();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_EFEDD110_E100_4081_8C2E_8D22E3C5C5E8_NodeOperation_h */
