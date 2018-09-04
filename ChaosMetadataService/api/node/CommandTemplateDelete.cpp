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

#include "CommandTemplateDelete.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_CTG_INFO INFO_LOG(CommandTemplateDelete)
#define N_CTG_DBG  DBG_LOG(CommandTemplateDelete)
#define N_CTG_ERR  ERR_LOG(CommandTemplateDelete)

CHAOS_MDS_DEFINE_API_CLASS_CD(CommandTemplateDelete, "commandTemplateDelete");

CDWUniquePtr CommandTemplateDelete::execute(CDWUniquePtr api_data) {
    int err = 0;
    bool presence = false;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, N_CTG_ERR, -2, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_name", N_CTG_ERR, -3, "The attribute template_name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_CTG_ERR, -4, "The attribute for command unique id is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    const std::string template_name = api_data->getStringValue("template_name");
    const std::string command_unique_id = api_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
    //check if the command template is present
    if((err = n_da->checkCommandTemplatePresence(template_name,
                                                 command_unique_id,
                                                 presence))) {
        LOG_AND_TROW_FORMATTED(N_CTG_ERR, err, "Error checking the command template %1%(%2%) presence", %template_name%command_unique_id)
    }
    
    if(presence) {
        if((err = n_da->deleteCommandTemplate(template_name,
                                              command_unique_id))){
            LOG_AND_TROW_FORMATTED(N_CTG_ERR, err, "Error removing the command template %1%(%2%)", %template_name%command_unique_id)
        }
    }
    return CDWUniquePtr(result);
}
