/*
 *	RestoreSnapshotBatch.cpp
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

#include "RestoreSnapshotBatch.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::general;

#define G_RS_INFO INFO_LOG(RestoreSnapshotBatch)
#define G_RS_DBG  DBG_LOG(RestoreSnapshotBatch)
#define G_RS_ERR  ERR_LOG(RestoreSnapshotBatch)

DEFINE_MDS_COMAMND_ALIAS(RestoreSnapshotBatch)

RestoreSnapshotBatch::RestoreSnapshotBatch():
MDSBatchCommand(),
node_index(0),
restore_phase(NEW_RESTORE_REQUEST){}

RestoreSnapshotBatch::~RestoreSnapshotBatch() {}

// inherited method
void RestoreSnapshotBatch::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    
    CHECK_CDW_THROW_AND_LOG(data, G_RS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, "snapshot_name", G_RS_ERR, -2, "The name of the snapshot is mandatory")
    
    snapshot_name = data->getStringValue("snapshot_name");
    
    if((err = getDataAccess<mds_data_access::SnapshotDataAccess>()->getNodeInSnapshot(snapshot_name,
                                                                                      list_node_in_snapshot))) {
        LOG_AND_TROW_FORMATTED(G_RS_ERR, err, "Error getting the node for snapshot '%1%'", %snapshot_name)
    } else if(list_node_in_snapshot.size() == 0) {
        LOG_AND_TROW_FORMATTED(G_RS_ERR, -1, "No node found for snapshot '%1%'", %snapshot_name)
    }
}

// inherited method
void RestoreSnapshotBatch::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void RestoreSnapshotBatch::ccHandler() {
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    MDSBatchCommand::ccHandler();
    switch(restore_phase) {
        case NEW_RESTORE_REQUEST:{
            //fetch the information for the node
            if(node_index >= list_node_in_snapshot.size()) {
                restore_phase = NO_MORE_NODE;
                break;
            }
            if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(list_node_in_snapshot[node_index],
                                                                                          &tmp_ptr))){
                LOG_AND_TROW_FORMATTED(G_RS_ERR, err, "Error getting node information for node %1%", %list_node_in_snapshot[node_index])
            } else if(tmp_ptr == NULL){
                G_RS_ERR << "Node information not found for " << list_node_in_snapshot[node_index];
                node_index++;
                break;
            }
            
            std::auto_ptr<CDataWrapper> node_description(tmp_ptr);
            
            if( !node_description->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) ||
               !node_description->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) {
                G_RS_ERR << "Node information doesn't contains rpc data for " << list_node_in_snapshot[node_index];
                node_index++;
                break;
            }

            const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
            const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
            
            G_RS_INFO << "send restore rpc command to " << list_node_in_snapshot[node_index] << " on [" <<node_rpc_address<<","<<node_rpc_domain<<"]";
            
            restore_message.reset(new CDataWrapper());
            restore_message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, list_node_in_snapshot[node_index]);
            restore_message->addStringValue(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG, snapshot_name);
            
            restore_request = createRequest(node_rpc_address,
                                            node_rpc_domain,
                                            NodeDomainAndActionRPC::ACTION_NODE_RESTORE);
            restore_phase = MANAGE_RESTORE_REQUEST;
            break;
        }
            
        case MANAGE_RESTORE_REQUEST:{
            switch(restore_request->phase) {
                case MESSAGE_PHASE_UNSENT: {
                    sendMessage(*restore_request,
                                restore_message.release());
                    
                }
                    
                case MESSAGE_PHASE_SENT: {
                    manageRequestPhase(*restore_request);
                    break;
                }
                    
                case MESSAGE_PHASE_COMPLETED:
                case MESSAGE_PHASE_TIMEOUT: {
                    //increment node index
                    node_index++;
                    
                    //switch to nex ndoe phase
                    restore_phase = NEW_RESTORE_REQUEST;
                    break;
                    
                default:
                    break;
                }
            }
            break;
        }
            
        case NO_MORE_NODE:{
            BC_END_RUNNING_PROPERTY
            break;
        }
    }
    
}

// inherited method
bool RestoreSnapshotBatch::timeoutHandler() {
    return MDSBatchCommand::timeoutHandler();
}