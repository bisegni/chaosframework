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

#include "CreateNewSnapshot.h"

#include "../../batch/general/CreateSnapshotBatch.h"

#define S_CNS_INFO INFO_LOG(RestoreSnapshot)
#define S_CNS_DBG  DBG_LOG(RestoreSnapshot)
#define S_CNS_ERR  ERR_LOG(RestoreSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(CreateNewSnapshot, "createNewSnapshot")

CDWUniquePtr CreateNewSnapshot::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, S_CNS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_CNS_ERR, -2, "The snapshot_name key is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("snapshot_name"), S_CNS_ERR, -3, "snapshot_name needs to be a string")
    CHECK_KEY_THROW_AND_LOG(api_data, "node_list", S_CNS_ERR, -3, "The node_list key is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isVectorValue("node_list"), S_CNS_ERR, -4, "node_list needs to be an array of string")
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -5);
    
    int err = 0;
    std::string work_id;
    uint64_t command_id = 0;
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    if((err = s_da->snapshotCreateNewWithName(snapshot_name,
                                              work_id))) {
        LOG_AND_TROW(S_CNS_ERR, err, CHAOS_FORMAT("Error creating the snapshot %1% with error %2%",%snapshot_name%err));
    }
    api_data->addStringValue("work_id", work_id);
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::general::CreateSnapshotBatch),
                                                   api_data.release());
    return CDWUniquePtr();
}
