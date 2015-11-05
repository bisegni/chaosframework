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

#include "RecoverError.h"
#include "../../batch/control_unit/RecoverError.h"

#include <chaos/common/chaos_constants.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_RNU_INFO INFO_LOG(RecoverError)
#define CU_RNU_DBG  DBG_LOG(RecoverError)
#define CU_RNU_ERR  ERR_LOG(RecoverError)

RecoverError::RecoverError():
AbstractApi("recoverError"){
    
}

RecoverError::~RecoverError() {
    
}

CDataWrapper *RecoverError::execute(CDataWrapper *api_data,
                                    bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, CU_RNU_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_RNU_ERR, -2, "The ndk_uid key is mandatory")
    if(!api_data->isVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-3, "ndk_uid key need to be a vectoro of string", __PRETTY_FUNCTION__);
    
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    
    int                                         err                 = 0;
    uint64_t                                    command_id          = 0;
    bool                                        presence            = false;
    std::string                                 temp_node_uid;
    std::auto_ptr<CMultiTypeDataArrayWrapper>   control_unit_ids(api_data->getVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    std::vector<std::string>                    control_unit_to_recover;
    
    for(int idx = 0; idx < control_unit_ids->size(); idx++) {
        temp_node_uid = control_unit_ids->getStringElementAtIndex(idx);
        if((err = cu_da->checkPresence(temp_node_uid, presence))) {
            CU_RNU_ERR << "Error checking the presence of control unit" << temp_node_uid << "with code:"<<err;
        } else if(presence){
            control_unit_to_recover.push_back(temp_node_uid);
        } else {
            CU_RNU_ERR << "The control unit" << temp_node_uid << "is not present";
        }
    }
    
    if(control_unit_to_recover.size()) {
        std::auto_ptr<CDataWrapper> batch_data(new CDataWrapper());
        //we can launch the batch command to recover all control unit
        for(std::vector<std::string>::iterator it = control_unit_to_recover.begin();
            it != control_unit_to_recover.end();
            it++) {
            batch_data->appendStringToArray(*it);
        }
        batch_data->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
        
        //launch the batch command
        command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::control_unit::RecoverError)),
                                                       batch_data.release());
    }
    
    return NULL;
}
