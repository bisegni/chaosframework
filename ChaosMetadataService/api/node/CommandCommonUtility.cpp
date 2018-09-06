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
#include <chaos/common/chaos_types.h>
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;

#define N_CCU_INFO INFO_LOG(CommandCommonUtility)
#define N_CCU_DBG  DBG_LOG(CommandCommonUtility)
#define N_CCU_ERR  ERR_LOG(CommandCommonUtility)

void CommandCommonUtility::validateCommandTemplateToDescription(CDWShrdPtr command_description,
                                                                CDWShrdPtr command_template,
                                                                std::vector<AttributeRequested> *attribute_requested_by_template) throw(chaos::CException) {
    //check ifthe command has parameter
    CHECK_CDW_THROW_AND_LOG(command_description, N_CCU_ERR, -1, "Command description is mandatory")
    CHECK_CDW_THROW_AND_LOG(command_template, N_CCU_ERR, -1, "Command template is mandatory")
    
    //    CHECK_KEY_THROW_AND_LOG_FORMATTED(command_description,
    //                                      BatchCommandAndParameterDescriptionkey::BC_PARAMETERS, N_CCU_ERR, -1,
    //                                      "The key '%1%' is not present in command description", %BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)
    
    CHECK_KEY_THROW_AND_LOG(command_template,
                            "template_name", N_CCU_ERR, -2,
                            "The key 'template_name' is not present in command template")
    
    bool is_correct_type = true;
    const std::string command_alias = command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS);
    const std::string template_name = command_template->getStringValue("template_name");
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)) {
        //check parameter for type and mandatory rule
        CMultiTypeDataArrayWrapperSPtr parameter_list(command_description->getVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
        for(int idx = 0;
            idx < parameter_list->size();
            idx++) {
            //scan the single parameter
            ChaosUniquePtr<chaos::common::data::CDataWrapper> parameter_description(parameter_list->getCDataWrapperElementAtIndex(idx));
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
            bool is_in_template = command_template->hasKey(attribute_name);
            
            if(!is_in_template) {
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
                    
                case chaos::DataType::TYPE_CLUSTER:
                    is_correct_type = command_template->isJsonValue(attribute_name);
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
}

//! create an instance by submission, command and temaplte description
ChaosUniquePtr<chaos::common::data::CDataWrapper> CommandCommonUtility::createCommandInstanceByTemplateadnSubmissionDescription(const std::string& node_uid,
                                                                                                                                CDWShrdPtr command_submission,
                                                                                                                                CDWShrdPtr command_description,
                                                                                                                                CDWShrdPtr command_template_description) throw(chaos::CException) {
    bool is_correct_type = false;
    ChaosStringVector all_template_key;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
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
    command_template_description->getAllKey(all_template_key);
    //we need to copy all template key except the key that are null because are the key that has been requested to the user and so
    //have been forwarded with the template name
    for(ChaosStringVectorIterator it = all_template_key.begin();
        it != all_template_key.end();
        it++) {
        if(command_template_description->isNullValue(*it)) {
            N_CCU_DBG << "Removed from template null value key:" << *it;
            continue;
        }
        //we can add the key value
        command_template_description->copyKeyTo(*it, *result);
    }
    
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
                
             case chaos::DataType::TYPE_CLUSTER:
                is_correct_type = command_submission->isJsonValue(it->first);
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
