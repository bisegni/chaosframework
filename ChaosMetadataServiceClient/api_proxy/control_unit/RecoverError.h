/*
 *	RecoverError.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__RecoverError_h
#define __CHAOSFramework__RecoverError_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                //! return the current control unit dataset
                class RecoverError:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(RecoverError)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(RecoverError)
                public:
                    //! copy control unit instance
                    /*!
                     cCpy the instance identifyed by control unit and unit server id to another contorl unit
                     asociatin git to same or other unit server
                     */
                    ApiProxyResult execute(const std::vector<std::string>& cu_uids);
                };
            }
        }
    }
}

#endif /* RecoverError_h */
