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

#include <chaos_metadata_service_client/api_proxy/node/CommandTemplateSubmit.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;


API_PROXY_CD_DEFINITION(CommandTemplateSubmit,
                        "system",
                        "commandTemplateSubmit")

ApiProxyResult CommandTemplateSubmit::execute(const TemplateSubmissionList& submission_task_list) {
    CDWUniquePtr message(new CDataWrapper());
    for(TemplateSubmissionListConstIterator it = submission_task_list.begin();
        it != submission_task_list.end();
        it++) {
        //compose submiossion task
        CDWUniquePtr submission_task(new CDataWrapper);
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
    return callApi(message);
}

ApiProxyResult CommandTemplateSubmit::execute(const std::string& node_uid,
                                              const std::string& command_alias,
                                              const SubmissionRuleType::SubmissionRule submission_rule,
                                              const uint32_t priority,
                                              const uint64_t scheduler_steps_delay,
                                              const uint32_t submission_checker_steps_delay,
                                              chaos::common::data::CDWUniquePtr& slow_command_data){
     CDWUniquePtr message(new CDataWrapper());
    //this key need only to inform mds to redirect to node the slowcomand without porcess it
    message->addNullValue("direct_mode");
    // set the default slow command information
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    message->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, command_alias);
    message->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submission_rule);
    message->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32, (uint32_t) priority);
    
    if(scheduler_steps_delay) message->addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, scheduler_steps_delay);
    if(submission_checker_steps_delay) message->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, submission_checker_steps_delay);
    if(slow_command_data.get()) {
        message->appendAllElement(*slow_command_data);
    }
    //call api
    return callApi(message);
}
