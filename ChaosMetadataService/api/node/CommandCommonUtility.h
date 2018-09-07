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
                    static void validateCommandTemplateToDescription(chaos::common::data::CDWShrdPtr command_description,
                                                                     chaos::common::data::CDWShrdPtr command_template_description,
                                                                     std::vector<AttributeRequested> *attribute_requested_by_template) throw(chaos::CException);
                    
                    //! create an instance by submission, command and temaplte description
                    static ChaosUniquePtr<chaos::common::data::CDataWrapper> createCommandInstanceByTemplateadnSubmissionDescription(const std::string& node_uid,
                                                                                                                                    chaos::common::data::CDWShrdPtr command_submission,
                                                                                                                                    chaos::common::data::CDWShrdPtr command_description,
                                                                                                                                    chaos::common::data::CDWShrdPtr command_template_description) throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandCommonUtility__) */
