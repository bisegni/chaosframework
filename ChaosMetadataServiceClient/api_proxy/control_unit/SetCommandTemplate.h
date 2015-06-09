/*
 *	SetCommandTemplate.h
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

#ifndef __CHAOSFramework__SetCommandTemplate__
#define __CHAOSFramework__SetCommandTemplate__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <chaos/common/data/CDataWrapperKeyValueSetter.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                struct CommandTemplate {
                    std::string template_name;
                    std::string command_alias;
                    std::vector< boost::shared_ptr<common::data::CDataWrapperKeyValueSetter> > paramter_value;
                    uint32_t submission_rule;
                    uint32_t priority;
                    uint64_t schedule_step_delay;
                    uint32_t submission_retry_delay;
                    uint32_t execution_channel;
                };
                
                //! Set (insert or update) a template for control unit command
                class SetCommandTemplate:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SetCommandTemplate)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(SetCommandTemplate)
                public:
                    //!
                    /*!
                     */
                    ApiProxyResult execute(const std::string& control_unit_uid,
                                           const boost::shared_ptr<CommandTemplate>& template_configuration);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SetCommandTemplate__) */
