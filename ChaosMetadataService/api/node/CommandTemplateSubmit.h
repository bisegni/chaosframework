/*
 *	CommandTemplateSubmit.h
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
