/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__CopyInstance_h
#define __CHAOSFramework__CopyInstance_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                //! return the current control unit dataset
                class CopyInstance:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CopyInstance)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(CopyInstance)
                public:
                    //! copy control unit instance
                    /*!
                     cCpy the instance identifyed by control unit and unit server id to another contorl unit
                     asociatin git to same or other unit server
                     */
                    ApiProxyResult execute(const std::string& cu_src,
                                           const std::string& us_src,
                                           const std::string& cu_dst,
                                           const std::string& us_dst);
                };
            }
        }
    }
}

#endif /* CopyInstance_hpp */
