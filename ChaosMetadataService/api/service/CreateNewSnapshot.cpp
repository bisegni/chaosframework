/*
 *	CreateNewSnapshot.cpp
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

#include "CreateNewSnapshot.h"

#include "../../batch/general/CreateSnapshotBatch.h"

#define S_CNS_INFO INFO_LOG(RestoreSnapshot)
#define S_CNS_DBG  DBG_LOG(RestoreSnapshot)
#define S_CNS_ERR  ERR_LOG(RestoreSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CreateNewSnapshot::CreateNewSnapshot():
AbstractApi("createNewSnapshot"){}

CreateNewSnapshot::~CreateNewSnapshot() {}

chaos::common::data::CDataWrapper *CreateNewSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    uint32_t command_id = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, S_CNS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_CNS_ERR, -2, "The snapshot_name key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "node_list", S_CNS_ERR, -3, "The node_list key is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isVectorValue("node_list"), S_CNS_ERR, -4, "node_list needs to be an array of string")
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -5);

    
    std::auto_ptr<CDataWrapper> batch_data(new CDataWrapper());
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::general::CreateSnapshotBatch),
                                                   batch_data.release());
    return NULL;
}
