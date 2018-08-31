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

#include "DeleteSnapshot.h"

#define S_DS_INFO INFO_LOG(DeleteSnapshot)
#define S_DS_DBG  DBG_LOG(DeleteSnapshot)
#define S_DS_ERR  ERR_LOG(DeleteSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< ChaosSharedPtr<CDataWrapper> > ResultVector;

CHAOS_MDS_DEFINE_API_CD(DeleteSnapshot, deleteSnapshot)

CDWUniquePtr DeleteSnapshot::execute(CDWUniquePtr api_data) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, S_DS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_DS_ERR, -2, "The snapshot_name key is mandatory")
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -5);
    int err = 0;
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    if((err = s_da->snapshotDeleteWithName(snapshot_name))){
        LOG_AND_TROW_FORMATTED(S_DS_ERR, -6, "Error erasing the snapshot %1% with code %2%", %snapshot_name%err)
    }
    return NULL;
}
