/*
 *	CommandCommonUtility.h
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

#ifndef __CHAOSFramework__CommandCommonUtility__
#define __CHAOSFramework__CommandCommonUtility__

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>

#include <utility>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                //wrap attribute name and type in a standard pair
                typedef std::pair<std::string, int> AttributeRequested;
                
                //! utility class to share check login across the api
                class CommandCommonUtility {
                public:
                    
                    //! check the validity of the template by the command description
                    /*!
                     The command is scanned in the parameter list and the tempalte si check according to parameter
                     description. The vector 'attribute_requested_by_template' is filled with the parameter that needs to
                     be setuped withi the template
                     */
                    static void validateCommandTemplateToDescription(chaos::common::data::CDataWrapper *command_description,
                                                                     chaos::common::data::CDataWrapper *command_template_description,
                                                                     std::vector<AttributeRequested> *attribute_requested_by_template) throw(chaos::CException);
                    
                    //! create an instance by submission, command and temaplte description
                    static ChaosUniquePtr<chaos::common::data::CDataWrapper> createCommandInstanceByTemplateadnSubmissionDescription(const std::string& node_uid,
                                                                                                                                    chaos::common::data::CDataWrapper *command_submission,
                                                                                                                                    chaos::common::data::CDataWrapper *command_description,
                                                                                                                                    chaos::common::data::CDataWrapper *command_template_description) throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandCommonUtility__) */
