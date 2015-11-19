/*
 *	CUCommonUtility.cpp
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

#include "CUCommonUtility.h"

#include <chaos/common/global.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::persistence::data_access;

#define CUCU_INFO LOG_LOG(CUCommonUtility)
#define CUCU_DBG  DBG_LOG(CUCommonUtility)
#define CUCU_ERR  ERR_LOG(CUCommonUtility)


std::auto_ptr<CDataWrapper> CUCommonUtility::prepareRequestPackForLoadControlUnit(const std::string& cu_uid, ControlUnitDataAccess *cu_da) {
    CUCU_DBG << "Prepare autoload request for:" << cu_uid;
    int err = 0;
    CDataWrapper * tmp_ptr = NULL;
    std::auto_ptr<CDataWrapper> instance_description;
    std::auto_ptr<CDataWrapper> result_pack;
    if((err = cu_da->getInstanceDescription(cu_uid,
                                            &tmp_ptr))) {
        //we haven't found an instance for the node
        CUCU_ERR << "Erroe getting instance" << err;
    } else if(tmp_ptr != NULL){
        instance_description.reset(tmp_ptr);
        
        //we have instances the rpc port is got from the unit server input data of the command
        if(!instance_description->hasKey("control_unit_implementation")) {
            CUCU_ERR << "No implementaiton found";
        } else {
            CUCU_DBG << "Create the autoload datapack for:" << cu_uid;
            result_pack.reset(new CDataWrapper());
            //add cu id
            result_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
            //add cu type
            result_pack->addStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE, instance_description->getStringValue("control_unit_implementation"));
            //add driver description
            instance_description->copyKeyTo(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION, *result_pack);
            instance_description->copyKeyTo(ControlUnitNodeDefinitionKey::CONTROL_UNIT_LOAD_PARAM, *result_pack);
        }
    } else {
        CUCU_ERR << "No instance found";
    }
    return result_pack;
}

void CUCommonUtility::prepareAutoInitAndStartInAutoLoadControlUnit(const std::string& cu_uid,
                                                                   ControlUnitDataAccess *cu_da,
                                                                   CDataWrapper *auto_load_pack) {
    CUCU_DBG << "Prepare autoload request for:" << cu_uid;
    int err = 0;
    CDataWrapper * tmp_ptr = NULL;
    //if the control unit to load need also to be initilized and started we compose the startup command to achieve this
    if((err = cu_da->getInstanceDescription(cu_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(CUCU_ERR, err, "Error %1% durring fetch of instance for unit server %2%", %err%cu_da)
    } else if(tmp_ptr) {
        std::auto_ptr<CDataWrapper> auto_inst(tmp_ptr);
        bool auto_init = auto_inst->hasKey("auto_init")?auto_inst->getBoolValue("auto_init"):false;
        bool auto_start = auto_inst->hasKey("auto_start")?auto_inst->getBoolValue("auto_start"):false;
        
        if(auto_init || auto_start) {
            if(auto_init){
                std::auto_ptr<CDataWrapper> init_datapack(new CDataWrapper());
                init_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
                
                std::auto_ptr<CDataWrapper> init_message_datapack(new CDataWrapper());
                init_message_datapack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, NodeDomainAndActionRPC::ACTION_NODE_INIT);
                init_message_datapack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *init_datapack);
                
                auto_load_pack->appendCDataWrapperToArray(*init_message_datapack);
            }
            if(auto_start){
                std::auto_ptr<CDataWrapper> start_datapack(new CDataWrapper());
                start_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
                
                std::auto_ptr<CDataWrapper> start_message_datapack(new CDataWrapper());
                start_message_datapack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, NodeDomainAndActionRPC::ACTION_NODE_START);
                start_message_datapack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *start_datapack);
                auto_load_pack->appendCDataWrapperToArray(*start_message_datapack);
            }
            
            //finalize startup command array
            auto_load_pack->finalizeArrayForKey(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_STARTUP_COMMAND);
        }
    }
}



