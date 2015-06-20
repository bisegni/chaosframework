/*
 *	CommandTemplateSubmit.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSubmit.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;


API_PROXY_CD_DEFINITION(CommandTemplateSubmit,
                        "system",
                        "commandTemplateSubmit")

/*!
 
 */
ApiProxyResult CommandTemplateSubmit::execute(const TemplateSubmissionList& submission_task_list) {
    std::auto_ptr<CDataWrapper> message(new CDataWrapper());
    for(TemplateSubmissionListConstIterator it = submission_task_list.begin();
        it != submission_task_list.end();
        it++) {
        //compose submiossion task
        std::auto_ptr<CDataWrapper> submission_task(new CDataWrapper);
        submission_task->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, (*it)->node_unique_id);
        submission_task->addStringValue("template_name", (*it)->template_name);
        submission_task->addStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, (*it)->command_unique_id);
        for(ParameterSetterListConstIterator it_parameter = (*it)->parametrized_attribute_value.begin();
            it_parameter != (*it)->parametrized_attribute_value.end();
            it_parameter++) {
           (*it_parameter)->setTo(*submission_task);
        }
        message->appendCDataWrapperToArray(*submission_task);
    }
    message->finalizeArrayForKey("submission_task");
    //call api
    return callApi(message.release());
}