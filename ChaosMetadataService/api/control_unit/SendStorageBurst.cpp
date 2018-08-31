/*
 * Copyright 2012, 03/07/2018 INFN
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

#include "SendStorageBurst.h"
#include "../../batch/node/SendRpcCommand.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/structured/Dataset.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_RNU_INFO INFO_LOG(RecoverError)
#define CU_RNU_DBG  DBG_LOG(RecoverError)
#define CU_RNU_ERR  ERR_LOG(RecoverError)

CHAOS_MDS_DEFINE_API(SendStorageBurst, sendStorageBurst);

CDWUniquePtr SendStorageBurst::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, CU_RNU_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_RNU_ERR, -2, "The ndk_uid key is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG((!api_data->isVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID) ||
                                   !api_data->isStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)), CU_RNU_ERR, -3, "ndk_uid key need to be a vector of string");
    
    //check burst information
    DatasetBurstSDWrapper db_sdw;
    db_sdw.deserialize(api_data.get());
    CHECK_ASSERTION_THROW_AND_LOG(db_sdw().tag.size() != 0, CU_RNU_ERR, -4, "The tag of burst is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(db_sdw().type != chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeUndefined, CU_RNU_ERR, -5, "The type of burst is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(db_sdw().value.isValid(), CU_RNU_ERR, -6, "The value ofburst is mandatory");
    
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -7)
    
    int                 err                 = 0;
    uint64_t            command_id          = 0;
    bool                presence            = false;
    std::string         temp_node_uid;
    ChaosStringVector   control_unit_to_recover;
    
    if(api_data->isVector(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {
        CMultiTypeDataArrayWrapperSPtr control_unit_ids = api_data->getVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
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
    } else {
        const std::string node_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
        if((err = cu_da->checkPresence(node_uid, presence))) {
            CU_RNU_ERR << "Error checking the presence of control unit" << temp_node_uid << "with code:"<<err;
        } else if(presence){
            control_unit_to_recover.push_back(node_uid);
        } else {
            CU_RNU_ERR << "The control unit" << temp_node_uid << "is not present";
        }
    }
    
    CDWUniquePtr burst_ser = db_sdw.serialize();
    for(ChaosStringVectorIterator it = control_unit_to_recover.begin(),
        end = control_unit_to_recover.end();
        it!= end;
        it++){
        ChaosUniquePtr<chaos::common::data::CDataWrapper> batch_data(new CDataWrapper());

        batch_data->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, *it);
        batch_data->addStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, chaos::ControlUnitNodeDomainAndActionRPC::ACTION_STORAGE_BURST);
        batch_data->addCSDataValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *burst_ser);
        //launch the batch command
        CU_RNU_DBG<<"Sending Command:"<<batch_data->getCompliantJSONString();

        command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::node::SendRpcCommand)),
                                                       batch_data.release());
    }
    return CDWUniquePtr();
}
