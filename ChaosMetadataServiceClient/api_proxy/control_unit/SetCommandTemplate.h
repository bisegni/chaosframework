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

#include <vector>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                typedef std::vector< boost::shared_ptr<common::data::CDataWrapperKeyValueSetter> > ParameterSetterList;
                typedef ParameterSetterList::iterator                                              ParameterSetterListIterator;
                /*!
                 wrap the command template information
                 */
                struct CommandTemplate {
                    //! the representative name of the tempalte (unique)
                    std::string template_name;
                    //! the unique id of the command
                    std::string command_unique_id;
                    //!the setter for the paramter
                    ParameterSetterList parameter_value_list;
                    //!the submission rule
                    uint32_t submission_rule;
                    //! the priority of the command within the whait queue
                    uint32_t submission_priority;
                    //! the run schedule delay between a runa and the next(in microseconds)
                    uint64_t schedule_step_delay;
                    //! the delay between the submission retry, waithing a favorable current command state(in milliseconds)
                    uint32_t submission_retry_delay;
                    //! the execution channel where run the command
                    uint32_t execution_channel;
                };
                
                typedef std::vector< boost::shared_ptr<CommandTemplate> >                   TemplateList;
                typedef std::vector< boost::shared_ptr<CommandTemplate> >::iterator         TemplateListIterator;
                typedef std::vector< boost::shared_ptr<CommandTemplate> >::const_iterator   TemplateListConstIterator;
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
                    ApiProxyResult execute(const TemplateList& template_configuration);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SetCommandTemplate__) */
