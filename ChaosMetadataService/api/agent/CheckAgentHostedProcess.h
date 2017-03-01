/*
 *	CheckAgentHostedProcess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EDB722C3_BEC1_4A23_9FCE_0331DBD3F803_CheckAgentHostedProcess_h
#define __CHAOSFramework_EDB722C3_BEC1_4A23_9FCE_0331DBD3F803_CheckAgentHostedProcess_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! remove an association from an angen and a node
                /*!
                 
                 */
                class CheckAgentHostedProcess:
                public AbstractApi {
                    
                public:
                    CheckAgentHostedProcess();
                    ~CheckAgentHostedProcess();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_EDB722C3_BEC1_4A23_9FCE_0331DBD3F803_CheckAgentHostedProcess_h */
