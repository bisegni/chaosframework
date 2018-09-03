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

#include "StartStop.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_STASTO_INFO INFO_LOG(StartStop)
#define CU_STASTO_DBG  DBG_LOG(StartStop)
#define CU_STASTO_ERR  ERR_LOG(StartStop)

CHAOS_MDS_DEFINE_API_CLASS_CD(StartStop, startStop);

CDWUniquePtr StartStop::execute(CDWUniquePtr api_data) {
    if(!api_data) {LOG_AND_TROW(CU_STASTO_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_STASTO_ERR, -2, "The ndk_unique_id key is mandatory");}
    if(!api_data->hasKey("start")) {LOG_AND_TROW(CU_STASTO_ERR, -3, "The start key is mandatory");}
    
    int err                     = 0;
    uint64_t command_id         = 0;
    CDataWrapper *tmp_ptr       = NULL;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> cu_base_description(new CDataWrapper());
    
    const std::string cu_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    const bool start = api_data->getBoolValue("start");
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -1)
    
    //get default control unit node description
    if((err = n_da->getNodeDescription(cu_uid, &tmp_ptr))) {
        LOG_AND_TROW(CU_STASTO_ERR, err, boost::str(boost::format("Error fetching the control unit node dafault description for unique id:%1% with error %2%") % cu_uid % err));
    } else if(tmp_ptr == NULL) {
        LOG_AND_TROW(CU_STASTO_ERR, err, boost::str(boost::format("No control unit node dafault description found for unique id:%1% ") % cu_uid));
    }
    ChaosUniquePtr<chaos::common::data::CDataWrapper> cu_desk(tmp_ptr);
    CHECK_KEY_THROW_AND_LOG(cu_desk.get(), NodeDefinitionKey::NODE_RPC_ADDR, CU_STASTO_ERR, -4, "No rpc addres in the control unit descirption")
    CHECK_KEY_THROW_AND_LOG(cu_desk.get(), NodeDefinitionKey::NODE_RPC_DOMAIN, CU_STASTO_ERR, -5, "No rpc domain in the control unit descirption")
    
    //set the control unique id in the init datapack
    cu_base_description->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    //set the aciton type
    cu_base_description->addInt32Value("action", (start?1:2));
    //launch initilization in background
    command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::control_unit::IDSTControlUnitBatchCommand)),
                                                   cu_base_description.release());
    return CDWUniquePtr();
}
