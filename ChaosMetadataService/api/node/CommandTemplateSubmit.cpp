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

#include "CommandTemplateSubmit.h"
#include "CommandCommonUtility.h"

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
AbstractApi("commandTemplateSubmit"){
    
}

CommandTemplateSubmit::~CommandTemplateSubmit() {
    
}

CDataWrapper *CommandTemplateSubmit::execute(CDataWrapper *api_data,
                                             bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, N_CTS_ERR, -2, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "submission_task", N_CTS_ERR, -3, "The list of submission task is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> submission_task_list(api_data->getVectorValue("submission_task"));
    for(int idx =0;
        idx < submission_task_list->size();
        idx++) {
        boost::shared_ptr<CDataWrapper>  submission_task(submission_task_list->getCDataWrapperElementAtIndex(idx));
        processSubmissionTask(n_da, submission_task);
    }
    
    //if we have reaced this point we should have all instnace well formed
    
    return NULL;
}

void CommandTemplateSubmit::processSubmissionTask(NodeDataAccess *n_da,
                                                  boost::shared_ptr<CDataWrapper> submission_task) {

    boost::shared_ptr<CDataWrapper> instance_to_submit;
    
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), NodeDefinitionKey::NODE_UNIQUE_ID, N_CTS_ERR, -1, "The node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), "template_name", N_CTS_ERR, -2, "The name of tempalte is mandatory")
    CHECK_KEY_THROW_AND_LOG(submission_task.get(), BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_CTS_ERR, -3, "The command unique id is mandatory")
    
    const std::string node_uid = submission_task->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string template_name = submission_task->getStringValue("template_name");
    const std::string command_unique_id = submission_task->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
       //fetch command
    boost::shared_ptr<CDataWrapper> command_description = getCommandDescription(n_da,
                                                                                command_unique_id);
        //fetch template
    boost::shared_ptr<CDataWrapper> template_description = getCommandTemaplateDescription(n_da,
                                                                                          template_name,
                                                                                          command_unique_id);
    
    //store command instance
    comamnd_instance_list.push_back(CommandCommonUtility::createCommandInstanceByTemplateadnSubmissionDescription(submission_task.get(),
                                                                                                                  command_description.get(),
                                                                                                                  template_description.get()));
}

boost::shared_ptr<CDataWrapper> CommandTemplateSubmit::getCommandDescription(NodeDataAccess *n_da,
                                                                             const std::string& command_unique_id) {
    int err = 0;
    bool presence = false;
    CDataWrapper *tmp_d_ptr = NULL;
    boost::shared_ptr<CDataWrapper> result;
    if(command_description_cache.count(command_unique_id)) {
        return command_description_cache[command_unique_id];
    }
    
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
    command_description_cache.insert(make_pair(command_unique_id, result = boost::shared_ptr<CDataWrapper>(tmp_d_ptr)));
    return result;
}

boost::shared_ptr<CDataWrapper> CommandTemplateSubmit::getCommandTemaplateDescription(NodeDataAccess *n_da,
                                                                                      const std::string& template_name,
                                                                                      const std::string& command_unique_id) {
    int err = 0;
    bool presence = false;
    CDataWrapper *tmp_d_ptr = NULL;
    boost::shared_ptr<CDataWrapper> result;
    TemplateKey template_key(template_name,
                             command_unique_id);
    
    if(template_description_cache.count(template_key)) {
        return template_description_cache[template_key];
    }
    
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
    template_description_cache.insert(make_pair(template_key, result = boost::shared_ptr<CDataWrapper>(tmp_d_ptr)));
    return result;
}