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

#include <chaos_metadata_service_client/api_proxy/node/CommandTemplateSet.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;


API_PROXY_CD_DEFINITION(CommandTemplateSet,
                        "system",
                        "commandTemplateSet")

/*!
 
 */
ApiProxyResult CommandTemplateSet::execute(const TemplateList& template_configurations) {
    CDWUniquePtr message(new CDataWrapper());
    for(TemplateListConstIterator it = template_configurations.begin();
        it != template_configurations.end();
        it++) {
        CDWUniquePtr template_element(new CDataWrapper());
        template_element->addStringValue("template_name", (*it)->template_name);
        template_element->addStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, (*it)->command_unique_id);
        //! scan all parameter configuration
        for(ParameterSetterListIterator it_param = (*it)->parameter_value_list.begin();
            it_param != (*it)->parameter_value_list.end();
            it_param++){
            (*it_param)->setTo(*template_element);
        }
        template_element->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (*it)->submission_rule);
        template_element->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32, (*it)->submission_priority);
        template_element->addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, (*it)->schedule_step_delay);
        template_element->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, (*it)->submission_retry_delay);
        template_element->addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (*it)->execution_channel);
        message->appendCDataWrapperToArray(*template_element);
    }
    message->finalizeArrayForKey("template_list");
    return callApi(message);
}
