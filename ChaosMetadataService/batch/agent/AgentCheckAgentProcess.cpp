/*
 *	AgentCheckAgentProcess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/02/2017 INFN, National Institute of Nuclear Physics
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

#include "AgentCheckAgentProcess.h"

#include "../../common/CUCommonUtility.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::agent;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::agent;

#define INFO INFO_LOG(AgentCheckAgentProcess)
#define DBG  DBG_LOG(AgentCheckAgentProcess)
#define ERR  ERR_LOG(AgentCheckAgentProcess)

DEFINE_MDS_COMAMND_ALIAS(AgentCheckAgentProcess)

AgentCheckAgentProcess::AgentCheckAgentProcess():
MDSBatchCommand(),
agent_uid(),
alert_event_channel(NULL){
alert_event_channel = NetworkBroker::getInstance()->getNewAlertEventChannel();
}

AgentCheckAgentProcess::~AgentCheckAgentProcess() {
if(alert_event_channel) {NetworkBroker::getInstance()->disposeEventChannel(alert_event_channel);}
}

// inherited method
void AgentCheckAgentProcess::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    CHECK_CDW_THROW_AND_LOG(data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The unique id of unit server is mandatory")
    
    int err = 0;
    std::string agent_host;
    agent_uid = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    //! fetch the agent information
    CDataWrapper *tmp_ptr;
    std::auto_ptr<CDataWrapper> agent_node_information;
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(agent_uid, &tmp_ptr))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error fetching node decription for %1%", %agent_uid))
    }
    agent_node_information.reset(tmp_ptr);
    
    
    AgentInstanceSDWrapper agent_description_sdw;
    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->loadAgentDescription(agent_uid,
                                                                                      true,
                                                                                      agent_description_sdw()))) {
        LOG_AND_TROW(ERR, -3, CHAOS_FORMAT("Error loading the description for agent %1% with error %2%", %agent_uid%err));
    }
    VectorAgentAssociationSDWrapper node_ass_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, agent_description_sdw().node_associated));
    node_ass_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    message_data = node_ass_sd_wrapper.serialize();
    
    request = createRequest(agent_node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                            AgentNodeDomainAndActionRPC::ProcessWorker::WORKER_NAME,
                            AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_CHECK_NODE);
}

// inherited method
void AgentCheckAgentProcess::acquireHandler() {
    
}

// inherited method
void AgentCheckAgentProcess::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendRequest(*request,
                        message_data.get());
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
        case MESSAGE_PHASE_COMPLETED: {
            if(request->request_future->wait(1000000) &&
               request->request_future->getResult() != NULL) {
                //we have answer from agent so we can update check information
                VectorAgentAssociationStatusSDWrapper status_vec_sd_wrapper;
                status_vec_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
                status_vec_sd_wrapper.deserialize(request->request_future->getResult());
                for(VectorAgentAssociationStatusIterator it = status_vec_sd_wrapper().begin(),
                    end = status_vec_sd_wrapper().end();
                    it != end;
                    it++) {
                    getDataAccess<mds_data_access::AgentDataAccess>()->setNodeAssociationStatus(agent_uid, *it);
                }

            }
            //send an alerto to inform all listener that hase benn done a cross check
            //we can send the event
            if(alert_event_channel){
                //send broadcast event
                alert_event_channel->sendAgentProcessCheckAlert(agent_uid, 1);
            }
        }
        case MESSAGE_PHASE_TIMEOUT:
        default:
            BC_END_RUNNING_PROPERTY;
            break;
    }
}

// inherited method
bool AgentCheckAgentProcess::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
