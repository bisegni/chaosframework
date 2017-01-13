/*
 *	DeleteSnapshot.h
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

#include "DeleteSnapshot.h"

#define S_DS_INFO INFO_LOG(DeleteSnapshot)
#define S_DS_DBG  DBG_LOG(DeleteSnapshot)
#define S_DS_ERR  ERR_LOG(DeleteSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< boost::shared_ptr<CDataWrapper> > ResultVector;

DeleteSnapshot::DeleteSnapshot():
AbstractApi("deleteSnapshot"){}

DeleteSnapshot::~DeleteSnapshot() {}

chaos::common::data::CDataWrapper *DeleteSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    
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
