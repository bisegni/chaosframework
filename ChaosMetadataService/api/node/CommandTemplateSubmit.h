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

#ifndef __CHAOSFramework__CommandTemplateSubmit__
#define __CHAOSFramework__CommandTemplateSubmit__

#include "../AbstractApi.h"

#include <chaos/common/chaos_types.h>

#include <map>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                
                CHAOS_DEFINE_PTR_VECTOR_FOR_TYPE(chaos::common::data::CDataWrapper, CommandInstanceList)
                
                class CommandTemplateSubmit:
                public AbstractApi {
                    
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> getCommandDescription(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                          const std::string& command_uid);
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> getCommandTemaplateDescription(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                                   const std::string& template_name,
                                                                                   const std::string& command_uid);
                    
                    void processSubmissionTask(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                               ChaosSharedPtr<chaos::common::data::CDataWrapper> submission_task,
                                               CommandInstanceList& command_instance_list);
                public:
                    CommandTemplateSubmit();
                    ~CommandTemplateSubmit();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandTemplateSubmit__) */
