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

#include "GetCommandAndTemplateDescription.h"
#include "CommandCommonUtility.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_GCTD_INFO INFO_LOG(GetCommandAndTemplateDescription)
#define N_GCTD_DBG  DBG_LOG(GetCommandAndTemplateDescription)
#define N_GCTD_ERR  ERR_LOG(GetCommandAndTemplateDescription)

CHAOS_MDS_DEFINE_API_CLASS_CD(GetCommandAndTemplateDescription, "getCommandAndTemplateDescription");

CDWUniquePtr GetCommandAndTemplateDescription::execute(CDWUniquePtr api_data) {
    int err = 0;
    CDataWrapper *tmp_d_ptr = NULL;
    CDWShrdPtr cmd_desc;
    CDWShrdPtr tmplt_cmd_desc;
    CHECK_CDW_THROW_AND_LOG(api_data, N_GCTD_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_name", N_GCTD_ERR, -2, "The name of the template is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_GCTD_ERR, -3, "The unique id of the command is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    
    const std::string template_name = api_data->getStringValue("template_name");
    const std::string command_unique_id = api_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    tmp_d_ptr = NULL;
    //load command description for validation
    if((err = n_da->getCommand(command_unique_id,
                               &tmp_d_ptr))) {
        LOG_AND_TROW_FORMATTED(N_GCTD_ERR, -5, "Error checking the presence of the for command uid %1%", %command_unique_id)
    } else if(!tmp_d_ptr) {
        LOG_AND_TROW_FORMATTED(N_GCTD_ERR, -6, "The uid %1% not have any command associated to it", %command_unique_id)
    } else {
        cmd_desc.reset(tmp_d_ptr);
        tmp_d_ptr = NULL;
    }
    
    //check if it is presence, otherwhise we need to add the sequence
    if((err = n_da->getCommandTemplate(template_name,
                                       command_unique_id,
                                       &tmp_d_ptr))){
        LOG_AND_TROW_FORMATTED(N_GCTD_ERR, -7, "Error checking the presence of the template %1% for command uid %2%", %template_name%command_unique_id)
    } else if(!tmp_d_ptr) {
        LOG_AND_TROW_FORMATTED(N_GCTD_ERR, -8, "The template '%1%' for the command uid '%2%' is not present", %template_name%command_unique_id)
    } else {
        tmplt_cmd_desc.reset(tmp_d_ptr);
    }
    
    //validate template with command
    CommandCommonUtility::validateCommandTemplateToDescription(cmd_desc, tmplt_cmd_desc, NULL);
    
    //we have either
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
    result->addCSDataValue("command_description", *cmd_desc);
    result->addCSDataValue("template_description", *tmplt_cmd_desc);
    return result;
}
