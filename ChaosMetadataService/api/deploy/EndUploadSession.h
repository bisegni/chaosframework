/*
 *	EndUploadSession.h
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

#ifndef __CHAOSFramework_D7291FD4_56CD_4E2D_A7D4_E026C7FFFBF0_EndUploadSession_h
#define __CHAOSFramework_D7291FD4_56CD_4E2D_A7D4_E026C7FFFBF0_EndUploadSession_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace deploy {
                
                //! remove an association from an angen and a node
                /*!
                 
                 */
                class EndUploadSession:
                public AbstractApi {
                    
                public:
                    EndUploadSession();
                    ~EndUploadSession();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_D7291FD4_56CD_4E2D_A7D4_E026C7FFFBF0_EndUploadSession_h */
