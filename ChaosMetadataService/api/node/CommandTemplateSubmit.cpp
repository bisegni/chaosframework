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

#include "CommandCommonUtility.h"
#include "CommandTemplateSubmit.h"
#include "../../batch/node/SendRpcCommand.h"
#include "../../batch/node/SubmitBatchCommand.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_CTS_INFO INFO_LOG(CommandTemplateSubmit)
#define N_CTS_DBG  DBG_LOG(CommandTemplateSubmit)
#define N_CTS_ERR  ERR_LOG(CommandTemplateSubmit)


CommandTemplateSubmit::CommandTemplateSubmit():
AbstractApi("commandTemplateSubmit"){}

CommandTemplateSubmit::~CommandTemplateSubmit() {}

CDWUniquePtr CommandTemplateSubmit::execute(CDWUniquePtr api_data) {
    uint64_t command_id = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, N_CTS_ERR, -1, "No parameter found");
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -2)
    
    if(api_data->hasKey("direct_mode") &&
       api_data->isNullValue("direct_mode")){
        // command is a raw command
        CHECK_KEY_THROW_AND_LOG(api_data,
                                NodeDefinitionKey::NODE_UNIQUE_ID,
                                N_CTS_ERR,
                                -3,
                                "The list of submission task is mandatory")
        N_CTS_DBG <<"Direct command submition:"<<api_data->getJSONString();
        
        CreateNewDataWrapper(rpc_command,);
        rpc_command->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                                    rpc_command->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
        rpc_command->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME,
                                    ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET);
        
        rpc_command->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *api_data);
        
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::SendRpcCommand),
                                                       rpc_command.release());
        return CDWUniquePtr();

    }
    CHECK_KEY_THROW_AND_LOG(api_data, "submission_task", N_CTS_ERR, -4, "The list of submission task is mandatory")

    //list of command instance
    CommandInstanceList command_instance_list;
    
    N_CTS_DBG << "validate all command instance";
    CMultiTypeDataArrayWrapperSPtr submission_task_list(api_data->getVectorValue("submission_task"));
    for(int idx =0;
        idx < submission_task_list->size();
        idx++) {
        ChaosSharedPtr<CDataWrapper>  submission_task(submission_task_list->getCDataWrapperElementAtIndex(idx));
        //!process the instance
        processSubmissionTask(n_da,
                              submission_task,
                              command_instance_list);
    }
    
    //if we have reaced this point we should have all instance well formed
    //so we can forward to respective node
    N_CTS_DBG << "Forward all command instance";
    for(CommandInstanceListIterator it = command_instance_list.begin();
        it != command_instance_list.end();
        it++) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> instance_pack(new CDataWrapper());
        N_CTS_INFO << "Send datapack "<< (*it)->getJSONString();
        instance_pack->addCSDataValue("submission_task", *(*it));
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::SubmitBatchCommand),
                                                       instance_pack.release());
    }
    return CDWUniquePtr();
}

void CommandTemplateSubmit::processSubmissionTask(NodeDataAccess *n_da,
                                                  ChaosSharedPtr<CDataWrapper> submission_task,
                                                  CommandInstanceList& command_instance_list) {
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), NodeDefinitionKey::NODE_UNIQUE_ID, N_CTS_ERR, -1, "The node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), "template_name", N_CTS_ERR, -2, "The name of tempalte is mandatory")
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_CTS_ERR, -3, "The command unique id is mandatory")
    
    const std::string node_uid = submission_task->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string template_name = submission_task->getStringValue("template_name");
    const std::string command_unique_id = submission_task->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
    //fetch command
    CDWShrdPtr command_description = getCommandDescription(n_da,
                                                           command_unique_id);
    //fetch template
    CDWShrdPtr template_description = getCommandTemaplateDescription(n_da,
                                                                     template_name,
                                                                     command_unique_id);
    
    //store command instance
    CDWShrdPtr instance = CommandCommonUtility::createCommandInstanceByTemplateadnSubmissionDescription(node_uid,
                                                                                                        submission_task,
                                                                                                        command_description,
                                                                                                        template_description);
    command_instance_list.push_back(instance);
}

ChaosSharedPtr<CDataWrapper> CommandTemplateSubmit::getCommandDescription(NodeDataAccess *n_da,
                                                                          const std::string& command_unique_id) {
    int err = 0;
    bool presence = false;
    CDataWrapper *tmp_d_ptr = NULL;
    ChaosSharedPtr<CDataWrapper> result;
    
    //we need to load the command
    if((err = n_da->checkCommandPresence(command_unique_id,
                                         presence))) {
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, err, "Error checking the command '%1%' presence", %command_unique_id)
    } else if(!presence) {
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, -4, "The command unique id '%1%' is not present", %command_unique_id)
    }
    
    if((err = n_da->getCommand(command_unique_id,
                               &tmp_d_ptr)) ||
       (tmp_d_ptr == NULL)) {
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, err, "Error loading the command '%1%'", %command_unique_id)
    }
    result.reset(tmp_d_ptr);
    return result;
}

ChaosSharedPtr<CDataWrapper> CommandTemplateSubmit::getCommandTemaplateDescription(NodeDataAccess *n_da,
                                                                                   const std::string& template_name,
                                                                                   const std::string& command_unique_id) {
    int err = 0;
    bool presence = false;
    CDataWrapper *tmp_d_ptr = NULL;
    ChaosSharedPtr<CDataWrapper> result;
    
    //we need to load the template
    if((err = n_da->checkCommandTemplatePresence(template_name,
                                                 command_unique_id,
                                                 presence))) {
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, err, "Error checking the command template %1%(%2%) presence", %template_name%command_unique_id)
    } else if(!presence) {
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, -4, "The tempalte '%1%' for command unique id '%2%' is not present", %template_name%command_unique_id)
    }
    
    if((err = n_da->getCommandTemplate(template_name,
                                       command_unique_id,
                                       &tmp_d_ptr)) ||
       (tmp_d_ptr == NULL)){
        LOG_AND_TROW_FORMATTED(N_CTS_ERR, err, "Error loading the command template %1%(%2%)", %template_name%command_unique_id)
    }
    result.reset(tmp_d_ptr);
    return result;
}
