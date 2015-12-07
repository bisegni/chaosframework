/*
 *	RestoreSnapshot.cpp
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

#include "RestoreSnapshot.h"
#include "../../batch/general/RestoreSnapshotBatch.h"

#define S_RS_INFO INFO_LOG(RestoreSnapshot)
#define S_RS_DBG  DBG_LOG(RestoreSnapshot)
#define S_RS_ERR  ERR_LOG(RestoreSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< boost::shared_ptr<CDataWrapper> > ResultVector;

RestoreSnapshot::RestoreSnapshot():
AbstractApi("restoreSnapshot"){}

RestoreSnapshot::~RestoreSnapshot() {}

chaos::common::data::CDataWrapper *RestoreSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, S_RS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_RS_ERR, -2, "The snapshot_name key is mandatory")
    
    std::auto_ptr<CDataWrapper> data_pack(new CDataWrapper());
    api_data->copyKeyTo("snapshot_name", *data_pack);
    
    //launch initilization in background
    uint64_t command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::general::RestoreSnapshotBatch)),
                                                            data_pack.release());
    
    S_RS_INFO << "Submitted restore for snapshot " << api_data->getStringValue("snapshot_name") << " with id:" << command_id;
    return NULL;
}
