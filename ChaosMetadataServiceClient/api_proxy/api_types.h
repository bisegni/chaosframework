/*
 *	api_types.h
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
#ifndef CHAOSFramework_api_types_h
#define CHAOSFramework_api_types_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataWrapperKeyValueSetter.h>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                //create list types for cdatawrapper setter
                CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<chaos::common::data::CDataWrapperKeyValueSetter>, ParameterSetterList)
                /*!
                 wrap the command template information
                 */
                struct CommandTemplate {
                    //! the representative name of the template (unique)
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

                CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<CommandTemplate>, TemplateList)
                
                struct TemplateSubmission {
                    //! the representative name of the template (unique)
                    std::string template_name;
                    //! the unique id of the command
                    std::string command_unique_id;
                    //! value for parametrized attribute in tempalte
                    ParameterSetterList parametrized_attribute_value;
                };
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<TemplateSubmission>, TemplateSubmissionList)
                
                //define the list, iterator and cost iterator
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, CommandUIDList)

            }
        }
    }
}


#endif
