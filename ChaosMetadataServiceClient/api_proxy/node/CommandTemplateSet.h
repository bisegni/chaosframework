/*
 *	CommandTemplateSet.h
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

#ifndef __CHAOSFramework__CommandTemplateSet__
#define __CHAOSFramework__CommandTemplateSet__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <chaos/common/data/CDataWrapperKeyValueSetter.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! Set (insert or update) a template for control unit command
                class CommandTemplateSet:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CommandTemplateSet)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(CommandTemplateSet)
                public:
                    //!
                    /*!
                     */
                    ApiProxyResult execute(const TemplateList& template_configuration);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandTemplateSet__) */
