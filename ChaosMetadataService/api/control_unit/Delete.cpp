/*
 *	RecoverError.h
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

#include "Delete.h"

#include <chaos/common/chaos_constants.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_D_INFO INFO_LOG(Delete)
#define CU_D_DBG  DBG_LOG(Delete)
#define CU_D_ERR  ERR_LOG(Delete)

Delete::Delete():
AbstractApi("delete"){
    
}

Delete::~Delete() {
    
}

CDataWrapper *Delete::execute(CDataWrapper *api_data,
                              bool& detach_data) throw(chaos::CException) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, CU_D_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_D_ERR, -2, "The ndk_uid key is mandatory")
    if(!api_data->isStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-3, "ndk_uid key need to be a string", __PRETTY_FUNCTION__);
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    const std::string node_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    if((err = n_da->deleteNode(node_uid,
                              chaos::NodeType::NODE_TYPE_CONTROL_UNIT))){
        LOG_AND_TROW_FORMATTED(CU_D_ERR, err, "Error deleting the control unit %1%", %node_uid);
    }
    return NULL;
}