/*
 *	DeployDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h
#define __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                //!
                class DeployDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                    
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    DeployDataAccess();
                    
                    //!default destructor
                    ~DeployDataAccess();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h */
