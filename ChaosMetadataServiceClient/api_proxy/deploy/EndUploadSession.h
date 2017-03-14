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

#ifndef __CHAOSFramework__05EB286_B216_4E0D_9BA5_67288006CF89_EndUploadSession_h
#define __CHAOSFramework__05EB286_B216_4E0D_9BA5_67288006CF89_EndUploadSession_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace deploy {
                
                //!
                class EndUploadSession:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(EndUploadSession)
                protected:
                    API_PROXY_CD_DECLARATION(EndUploadSession)
                public:
                    //!
                    /*!
                     \param
                     */
                    ApiProxyResult execute(const std::string& agent_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__05EB286_B216_4E0D_9BA5_67288006CF89_EndUploadSession_h */
