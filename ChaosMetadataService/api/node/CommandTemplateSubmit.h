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

#include <map>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                
                typedef pair<std::string, std::string> TemplateKey;
                
                class CommandTemplateSubmit:
                public AbstractApi {
                    //list of command instance
                    std::vector<boost::shared_ptr<CDataWrapper> > comamnd_instance_list;
                    //cache the command description for reuse
                    std::map<std::string, boost::shared_ptr<CDataWrapper> > command_description_cache;
                    //cache the template description for reuse
                    std::map<TemplateKey, boost::shared_ptr<CDataWrapper> > template_description_cache;
                    
                    boost::shared_ptr<CDataWrapper> getCommandDescription(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                          const std::string& command_uid);
                    boost::shared_ptr<CDataWrapper> getCommandTemaplateDescription(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                                   const std::string& template_name,
                                                                                   const std::string& command_uid);
                    
                    void processSubmissionTask(chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                               boost::shared_ptr<chaos::common::data::CDataWrapper> submission_task);
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
