/*
 *	CommandTemplateSet.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/node/CommandTemplateSet.h>

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
    ChaosUniquePtr<CDataWrapper> message(new CDataWrapper());
    for(TemplateListConstIterator it = template_configurations.begin();
        it != template_configurations.end();
        it++) {
        ChaosUniquePtr<CDataWrapper> template_element(new CDataWrapper());
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
    //call api
    return callApi(message.release());
}
