/*
 *	CommandTemplateGet.cpp
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
#include "CommandTemplateGet.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_CTG_INFO INFO_LOG(CommandTemplateGet)
#define CU_CTG_DBG  DBG_LOG(CommandTemplateGet)
#define CU_CTG_ERR  ERR_LOG(CommandTemplateGet)

CommandTemplateGet::CommandTemplateGet():
AbstractApi("commandTemplateGet"){
    
}

CommandTemplateGet::~CommandTemplateGet() {
    
}

CDataWrapper *CommandTemplateGet::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    int err = 0;
    bool presence = false;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, CU_CTG_ERR, -2, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_name", CU_CTG_ERR, -3, "The attribute template_name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, CU_CTG_ERR, -4, "The attribute for command unique id is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    const std::string template_name = api_data->getStringValue("template_name");
    const std::string command_unique_id = api_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
    //check if the command template is present
    if((err = n_da->checkCommandTemplatePresence(template_name,
                                                 command_unique_id,
                                                 presence))) {
        LOG_AND_TROW_FORMATTED(CU_CTG_ERR, err, "Error checking the command template %1%(%2%) presence", %template_name%command_unique_id)
    }
    
    if(presence) {
        if((err = n_da->getCommandTemplate(template_name,
                                          command_unique_id,
                                          &result))){
            LOG_AND_TROW_FORMATTED(CU_CTG_ERR, err, "Error getting the command tempalte %1%(%2%)", %template_name%command_unique_id)
        }
    }
    return result;
}