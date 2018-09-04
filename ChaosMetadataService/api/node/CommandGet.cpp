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

#include "CommandGet.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_CG_INFO INFO_LOG(CommandGet)
#define N_CG_DBG  DBG_LOG(CommandGet)
#define N_CG_ERR  ERR_LOG(CommandGet)

CHAOS_MDS_DEFINE_API_CLASS_CD(CommandGet, "commandGet");

CDWUniquePtr CommandGet::execute(CDWUniquePtr api_data) {
    int err = 0;
    CDataWrapper *result = NULL;
    CHECK_CDW_THROW_AND_LOG(api_data, N_CG_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, N_CG_ERR, -2, "The attribute BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    const std::string command_unique_id = api_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    
    //we need to add the sequence
    if((err = n_da->getCommand(command_unique_id, &result))) {
        LOG_AND_TROW_FORMATTED(N_CG_ERR, -4, "Error getting command for uid %1%", %command_unique_id);
    }
    return CDWUniquePtr(result);
}
