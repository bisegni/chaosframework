/*
 *	CommandCommonUtility.cpp
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

#include "CommandCommonUtility.h"

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;

#define N_CCU_INFO INFO_LOG(CommandCommonUtility)
#define N_CCU_DBG  DBG_LOG(CommandCommonUtility)
#define N_CCU_ERR  ERR_LOG(CommandCommonUtility)

void CommandCommonUtility::validateCommandTemplateToDescription(CDataWrapper *command_description,
                                                                CDataWrapper *command_template,
                                                                std::vector<AttributeRequested> *attribute_requested_by_template) throw(chaos::CException) {
    //check ifthe command has parameter
    CHECK_CDW_THROW_AND_LOG(command_description, N_CCU_ERR, -1, "Command description is mandatory")
    CHECK_CDW_THROW_AND_LOG(command_template, N_CCU_ERR, -1, "Command template is mandatory")
    
    CHECK_KEY_THROW_AND_LOG_FORMATTED(command_description,
                                      BatchCommandAndParameterDescriptionkey::BC_PARAMETERS, N_CCU_ERR, -1,
                                      "The key '%1%' is not present in command description", %BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)
    
    CHECK_KEY_THROW_AND_LOG(command_template,
                            "template_name", N_CCU_ERR, -2,
                            "The key 'template_name' is not present in command template")
    
    bool is_correct_type = true;
    const std::string command_alias = command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS);
    const std::string template_name = command_template->getStringValue("template_name");
    //check parameter for type and mandatory rule
    std::auto_ptr<CMultiTypeDataArrayWrapper> parameter_list(command_description->getVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
    for(int idx = 0;
        idx < parameter_list->size();
        idx++) {
        //scan the single parameter
        std::auto_ptr<CDataWrapper> parameter_description(parameter_list->getCDataWrapperElementAtIndex(idx));
        //check for mandatory field in command description
        CHECK_KEY_THROW_AND_LOG_FORMATTED(parameter_description,
                                          BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME, N_CCU_ERR, -2,
                                          "The key '%1%' is not present in command parameter description", %BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME)
        CHECK_KEY_THROW_AND_LOG_FORMATTED(parameter_description,
                                          BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE, N_CCU_ERR, -3,
                                          "The key '%1%' is not present in command parameter description", %BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)
        CHECK_KEY_THROW_AND_LOG_FORMATTED(parameter_description,
                                          BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG, N_CCU_ERR, -4,
                                          "The key '%1%' is not present in command parameter description", %BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG)
        
        //check mandatory rule
        const std::string attribute_name = parameter_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME);
        const int flags = parameter_description->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG);
        bool is_in_tempalte = command_template->hasKey(attribute_name);
        
        if(!is_in_tempalte) {
            if((flags & BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY) ==
               BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY) {
                //the attribute is mandatory but not in the template so we fire the exception
                LOG_AND_TROW_FORMATTED(N_CCU_ERR, -6,
                                       "The attribute '%1%' fo command '%2%' is mandatory but is not present into the template '%3%'",
                                       %attribute_name%command_alias%template_name)
            } else {
                continue;
            }
        }
        
        //if the attribute is null meaning that it need to be forwarded
        if(command_template->isNullValue(attribute_name)) {
            if(attribute_requested_by_template)
                attribute_requested_by_template->push_back(make_pair(attribute_name, parameter_description->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)));
            continue;
        }
        //reset bool check variable
        is_correct_type = true;
        
        //check type
        switch (parameter_description->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)) {
            case chaos::DataType::TYPE_BOOLEAN:
                is_correct_type = command_template->isBoolValue(attribute_name);
                break;
            case chaos::DataType::TYPE_INT32:
                is_correct_type = command_template->isInt32Value(attribute_name);
                break;
            case chaos::DataType::TYPE_INT64:
                is_correct_type = command_template->isInt64Value(attribute_name);
                break;
            case chaos::DataType::TYPE_DOUBLE:
                is_correct_type = command_template->isDoubleValue(attribute_name);
                break;
            case chaos::DataType::TYPE_STRING:
                is_correct_type = command_template->isStringValue(attribute_name);
                break;
            case chaos::DataType::TYPE_BYTEARRAY:
                is_correct_type = command_template->isBinaryValue(attribute_name);
                break;
        }
        if(!is_correct_type) {
            LOG_AND_TROW_FORMATTED(N_CCU_ERR, -6,
                                   "The attribute '%1%' for command '%2%' has incorrect type in template '%3%'",
                                   %attribute_name%command_alias%template_name)
        }
    }
}

//! create an instance by submission, command and temaplte description
std::auto_ptr<chaos::common::data::CDataWrapper> CommandCommonUtility::createCommandInstanceByTemplateadnSubmissionDescription(const std::string& node_uid,
                                                                                                                               CDataWrapper *command_submission,
                                                                                                                               CDataWrapper *command_description,
                                                                                                                               CDataWrapper *command_template_description) throw(chaos::CException) {
    bool is_correct_type = false;
    std::auto_ptr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
    result->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    
    std::vector<AttributeRequested> attribute_paramterized;
    //validate input parameter
    CHECK_CDW_THROW_AND_LOG(command_submission, N_CCU_ERR, -1, "The command submission is mandatory")
    
    validateCommandTemplateToDescription(command_description,
                                         command_template_description,
                                         &attribute_paramterized);
    
    const std::string template_name = command_template_description->getStringValue("template_name");
    
    //we have all parameter valid now compose the request
    command_description->copyKeyTo(BatchCommandAndParameterDescriptionkey::BC_ALIAS, *result);
    command_template_description->copyAllTo(*result);
    
    //try to find requested paramter whitin the command submission
    for(std::vector<AttributeRequested>::iterator it = attribute_paramterized.begin();
        it != attribute_paramterized.end();
        it++) {
        if(!command_submission->hasKey(it->first)) {
            LOG_AND_TROW_FORMATTED(N_CCU_ERR, -2,
                                   "The attribute '%1%' is mandatory by the template '%2%'",
                                   %it->first%template_name)
        }
        //check type
        switch (it->second) {
            case chaos::DataType::TYPE_BOOLEAN:
                is_correct_type = command_submission->isBoolValue(it->first);
                break;
            case chaos::DataType::TYPE_INT32:
                is_correct_type = command_submission->isInt32Value(it->first);
                break;
            case chaos::DataType::TYPE_INT64:
                is_correct_type = command_submission->isInt64Value(it->first);
                break;
            case chaos::DataType::TYPE_DOUBLE:
                is_correct_type = command_submission->isDoubleValue(it->first);
                break;
            case chaos::DataType::TYPE_STRING:
                is_correct_type = command_submission->isStringValue(it->first);
                break;
            case chaos::DataType::TYPE_BYTEARRAY:
                is_correct_type = command_submission->isBinaryValue(it->first);
                break;
        }
        
        if(!is_correct_type){
            LOG_AND_TROW_FORMATTED(N_CCU_ERR, -3,
                                   "The attribute '%1%' is not of the right type",
                                   %it->first)
        }
        
        if(!command_submission->copyKeyTo(it->first, *result)) {
            // if we are here meaning that a requested attribute within the tempalte is not present
            // into the submission object
            LOG_AND_TROW_FORMATTED(N_CCU_ERR, -3,
                                   "The attribute '%1%' is needde by the template '%2%'",
                                   %it->first%template_name)
        }
    }
    return result;
}