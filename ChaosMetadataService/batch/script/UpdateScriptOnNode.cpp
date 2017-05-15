/*
 *	UpdateScriptOnNode.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/04/2017 INFN, National Institute of Nuclear Physics
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

#include "UpdateScriptOnNode.h"

#include "../control_unit/IDSTControlUnitBatchCommand.h"
#include "../../common/CUCommonUtility.h"


using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::script;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch::script;

#define INFO INFO_LOG(UpdateScriptOnNode)
#define DBG  DBG_LOG(UpdateScriptOnNode)
#define ERR  ERR_LOG(UpdateScriptOnNode)

DEFINE_MDS_COMAMND_ALIAS(UpdateScriptOnNode)

UpdateScriptOnNode::UpdateScriptOnNode():
MDSBatchCommand(),
phase(UploadPhaseFetchNodeInfo){}

UpdateScriptOnNode::~UpdateScriptOnNode() {}

// inherited method
void UpdateScriptOnNode::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    if(data == NULL) {
        LOG_AND_TROW(ERR, -1, "No input data available");
    }
    
    if(data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID) == false) {
        LOG_AND_TROW(ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    }
    
    //deserialize base description of the script
    sdw.deserialize(data);
    if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->loadScript(sdw().script_description.unique_id,
                                                                             sdw().script_description.name,
                                                                             sdw()))){
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error loading the script %1%", %sdw().script_description.name));
    }
    //compose the update script datapack
    script_update_data_pack = sdw.serialize();
    
    //fiil the vector with the node uid
    if(data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        node_to_update_vec.push_back(data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
    } else if(data->isVectorValue(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> arr(data->getVectorValue(NodeDefinitionKey::NODE_UNIQUE_ID));
        for(int idx = 0;
            idx < arr->size();
            idx++) {
            node_to_update_vec.push_back(arr->getStringElementAtIndex(idx));
        }
        
    }
    //set the start of node uid iterator
    current_node_it = node_to_update_vec.begin();
    
    //for debug
    setFeatures(::chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)1000000000);
}

// inherited method
void UpdateScriptOnNode::acquireHandler() {
}

// inherited method
void UpdateScriptOnNode::ccHandler() {
    int err = 0;
    switch(phase) {
        case UploadPhaseFetchNodeInfo:{
            if(current_node_it == node_to_update_vec.end()) {
                //we have finish
                BC_END_RUNNING_PROPERTY
            } else {
                bool alive = false;
                CDataWrapper *tmp_ptr = NULL;
                std::auto_ptr<CDataWrapper> node_desc;
                //fetch new node description
                if((err = getDataAccess<mds_data_access::UnitServerDataAccess>()->getDescription(*current_node_it, &tmp_ptr)) ||
                   tmp_ptr == NULL) {
                    LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error loading the description for node %1%", %*current_node_it));
                }
                //copy script in node instance description
                prepareScriptInstance(sdw().script_description, *current_node_it);
                
                
                //check if the ndoe is online
                if((err = getDataAccess<mds_data_access::NodeDataAccess>()->isNodeAlive(*current_node_it,
                                                                                        alive))) {
                    LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error checking the online state for node %1%", %*current_node_it));
                }
                if(alive) {
                    //send node
                    node_desc.reset(tmp_ptr);
                    if(node_desc->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) &&
                       node_desc->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) {
                        //create the request for update the source on node
                        request = createRequest(node_desc->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR),
                                                node_desc->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN),
                                                "updateScriptSource");
                        phase = UploadPhaseSendToNode;
                    } else {
                        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("RPC information not found for node %1%", %*current_node_it));
                    }
                } else {
                    //go to next node
                    current_node_it++;
                }
            }
            break;
        }
            
        case UploadPhaseSendToNode: {
            if(loadScriptInstance()) {
                //go to next node
                current_node_it++;
                //set the phase for manage node
                phase = UploadPhaseFetchNodeInfo;
            }
            break;
        }
    }
}

// inherited method
bool UpdateScriptOnNode::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}

bool UpdateScriptOnNode::prepareScriptInstance(const chaos::service_common::data::script::ScriptBaseDescription& current_script_description,
                                               const std::string& node_uid) {
    int err = 0;
    
    //first time we need to copy the script dataset into the instance
    if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->copyScriptDatasetAndContentToInstance(current_script_description,
                                                                                                        node_uid))){
        ERR << "We have had error copying script dataset to instance";
        return true;
    }
    return true;
}

bool UpdateScriptOnNode::loadScriptInstance() {
    bool result = false;
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:{
            sendMessage(*request,
                        script_update_data_pack.get());
            result = true;
            break;
        }
            
        case MESSAGE_PHASE_SENT:{
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:{
            result = true;
            break;
        }
    }
    return result;
}
