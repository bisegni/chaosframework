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

#define S_CNS_INFO INFO_LOG(RestoreSnapshot)
#define S_CNS_DBG  DBG_LOG(RestoreSnapshot)
#define S_CNS_ERR  ERR_LOG(RestoreSnapshot)

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, NodeList)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< boost::shared_ptr<CDataWrapper> > ResultVector;

CreateNewSnapshot::CreateNewSnapshot():
AbstractApi("createNewSnapshot"){}

CreateNewSnapshot::~CreateNewSnapshot() {}

chaos::common::data::CDataWrapper *CreateNewSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, S_CNS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_CNS_ERR, -2, "The snapshot_name key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "node_list", S_CNS_ERR, -3, "The node_list key is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isVectorValue("node_list"), S_CNS_ERR, -4, "node_list needs to be an array of string")
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -5);
    int err = 0;
    NodeList node_list_to_include;
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    std:auto_ptr<CMultiTypeDataArrayWrapper> _node_list(api_data->getVectorValue("node_list"));
    for(int idx =0;
        idx < _node_list->size();
        idx++) {
        node_list_to_include.push_back(_node_list->getStringElementAtIndex(idx));
    }
    if(node_list_to_include.size()) {
        if((err = s_da->createNewSnapshot(snapshot_name, node_list_to_include))){
            LOG_AND_TROW_FORMATTED(S_CNS_ERR, -6, "Error creating new snapshot with code %1%", %err)
        }
    } else {
        LOG_AND_TROW(S_CNS_ERR, -6, "The node list of the nodes to include is empty")
    }

    return NULL;
}
