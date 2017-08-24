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

#include "RestoreSnapshot.h"
#include "../../batch/general/RestoreSnapshotBatch.h"

#define S_RS_INFO INFO_LOG(RestoreSnapshot)
#define S_RS_DBG  DBG_LOG(RestoreSnapshot)
#define S_RS_ERR  ERR_LOG(RestoreSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< ChaosSharedPtr<CDataWrapper> > ResultVector;

RestoreSnapshot::RestoreSnapshot():
AbstractApi("restoreSnapshot"){}

RestoreSnapshot::~RestoreSnapshot() {}

chaos::common::data::CDataWrapper *RestoreSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, S_RS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_RS_ERR, -2, "The snapshot_name key is mandatory")
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_pack(new CDataWrapper());
    api_data->copyKeyTo("snapshot_name", *data_pack);
    
    //launch initilization in background
    uint64_t command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::general::RestoreSnapshotBatch)),
                                                            data_pack.release());
    
    S_RS_INFO << "Submitted restore for snapshot " << api_data->getStringValue("snapshot_name") << " with id:" << command_id;
    return NULL;
}
