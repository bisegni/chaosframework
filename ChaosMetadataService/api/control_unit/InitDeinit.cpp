/*
 *	InitDeinit.cpp
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

#include "InitDeinit.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_INDEIN_INFO INFO_LOG(InitDeinit)
#define CU_INDEIN_DBG  DBG_LOG(InitDeinit)
#define CU_INDEIN_ERR  ERR_LOG(InitDeinit)

InitDeinit::InitDeinit():
AbstractApi("initDeinit"){
    
}

InitDeinit::~InitDeinit() {
    
}

CDataWrapper *InitDeinit::execute(CDataWrapper *api_data,
                                  bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, CU_INDEIN_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, CU_INDEIN_ERR, -2, "The ndk_unique_id key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "init", CU_INDEIN_ERR, -3, "The init key is mandatory")
    
    const std::string cu_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    const bool init = api_data->getBoolValue("init");
    
    int err = 0;
    uint64_t command_id;
    CDataWrapper *tmp_ptr = NULL;

    std::auto_ptr<CDataWrapper> data_pack(new CDataWrapper());
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -1)
    if((err = n_da->getNodeDescription(cu_uid, &tmp_ptr))) {
        LOG_AND_TROW(CU_INDEIN_ERR, err, boost::str(boost::format("Error fetching the control unit node dafault description for unique id:%1% with error %2%") % cu_uid % err));
    } else if(tmp_ptr == NULL) {
        LOG_AND_TROW(CU_INDEIN_ERR, err, boost::str(boost::format("No control unit node dafault description found for unique id:%1% ") % cu_uid));
    }
    std::auto_ptr<CDataWrapper> cu_desk(tmp_ptr);
    CHECK_KEY_THROW_AND_LOG(cu_desk.get(), NodeDefinitionKey::NODE_RPC_ADDR, CU_INDEIN_ERR, -4, "No rpc addres in the control unit descirption")
    CHECK_KEY_THROW_AND_LOG(cu_desk.get(), NodeDefinitionKey::NODE_RPC_DOMAIN, CU_INDEIN_ERR, -5, "No rpc domain in the control unit descirption")
    
    //set the control unique id in the init datapack
    data_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    
    if(init) {
        //set the aciton type
        data_pack->addInt32Value("action", (int32_t)0);
    } else {
        //set the aciton type
        data_pack->addInt32Value("action", (int32_t)3);
    }
    
    //launch initilization in background
    command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::control_unit::IDSTControlUnitBatchCommand)),
                                                   data_pack.release());
    return NULL;
}
