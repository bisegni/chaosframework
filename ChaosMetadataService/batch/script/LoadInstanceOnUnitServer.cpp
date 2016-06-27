/*
 *	LoadInstanceOnUnitServer.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/06/16 INFN, National Institute of Nuclear Physics
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

#include "LoadInstanceOnUnitServer.h"

#include "../control_unit/IDSTControlUnitBatchCommand.h"
#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch::script;

#define INFO INFO_LOG(LoadInstanceOnUnitServer)
#define DBG  DBG_LOG(LoadInstanceOnUnitServer)
#define ERR  ERR_LOG(LoadInstanceOnUnitServer)

DEFINE_MDS_COMAMND_ALIAS(LoadInstanceOnUnitServer)

LoadInstanceOnUnitServer::LoadInstanceOnUnitServer():
MDSBatchCommand() {}
LoadInstanceOnUnitServer::~LoadInstanceOnUnitServer() {}

// inherited method
void LoadInstanceOnUnitServer::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_CDW_THROW_AND_LOG(data, ERR, -1, "No parameter found");
    
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_PARENT, ERR, -2, "The node parent key for unit server is mandatory");
    CHAOS_LASSERT_EXCEPTION(data->isStringValue(chaos::NodeDefinitionKey::NODE_PARENT), ERR, -3, CHAOS_FORMAT("%1% key need to be a string with the unit server uid", %chaos::NodeDefinitionKey::NODE_PARENT));
    
    CHECK_KEY_THROW_AND_LOG(data, "ep_pool_list", ERR, -4, "The execution pool list is mandatory");
    CHAOS_LASSERT_EXCEPTION(data->isVectorValue("ep_pool_list"), ERR, -5, "ep_pool_list key need to be a vector with the execution pool list");

    unit_server = data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);
    std::auto_ptr<CMultiTypeDataArrayWrapper> array(data->getVectorValue("ep_pool_list"));
    for(int idx = 0;
        idx < array->size();
        idx++) {
        epool_list.push_back(array->getStringElementAtIndex(idx));
    }
    

    
        //set the send command phase
//    request = createRequest(us_address,
//                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
//                            (load?UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT:UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT));
}

// inherited method
void LoadInstanceOnUnitServer::acquireHandler() {
//    MDSBatchCommand::acquireHandler();
//    switch(request->phase) {
//        case MESSAGE_PHASE_UNSENT:{
//            sendMessage(*request,
//                        load_unload_pack.get());
//            BC_END_RUNNIG_PROPERTY
//            break;
//        }
//        case MESSAGE_PHASE_SENT:
//        case MESSAGE_PHASE_COMPLETED:
//        case MESSAGE_PHASE_TIMEOUT:
//            break;
//    }
}

// inherited method
void LoadInstanceOnUnitServer::ccHandler() {
//    MDSBatchCommand::ccHandler();
//    switch(request->phase) {
//        case MESSAGE_PHASE_UNSENT:
//            break;
//            
//        case MESSAGE_PHASE_SENT:{
//            manageRequestPhase(*request);
//            break;
//        }
//            
//        case MESSAGE_PHASE_COMPLETED:
//        case MESSAGE_PHASE_TIMEOUT:{
//            BC_END_RUNNIG_PROPERTY
//            break;
//        }
//    }
}

// inherited method
bool LoadInstanceOnUnitServer::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}