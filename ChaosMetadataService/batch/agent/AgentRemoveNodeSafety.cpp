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

#include "AgentRemoveNodeSafety.h"

#include "../../common/CUCommonUtility.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::agent;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::agent;

#define INFO INFO_LOG(AgentRemoveNodeSafety)
#define DBG  DBG_LOG(AgentRemoveNodeSafety)
#define ERR  ERR_LOG(AgentRemoveNodeSafety)

DEFINE_MDS_COMAMND_ALIAS(AgentRemoveNodeSafety)

AgentRemoveNodeSafety::AgentRemoveNodeSafety():
MDSBatchCommand(),
scan_phase(ScanPhaseNext),
agent_uid(),
associated_nodes(),
alert_event_channel(NULL){
    alert_event_channel = NetworkBroker::getInstance()->getNewAlertEventChannel();
}

AgentRemoveNodeSafety::~AgentRemoveNodeSafety() {
    if(alert_event_channel) {NetworkBroker::getInstance()->disposeEventChannel(alert_event_channel);}
}

// inherited method
void AgentRemoveNodeSafety::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    CHECK_ASSERTION_THROW_AND_LOG((data!=NULL), ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    CHECK_KEY_THROW_AND_LOG(data, chaos::AgentNodeDefinitionKey::NODE_ASSOCIATED, ERR, -3, CHAOS_FORMAT("The %1% key is mandatory",%chaos::AgentNodeDefinitionKey::NODE_ASSOCIATED));
    int err = 0;
    std::string agent_host;
    agent_uid = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if(data->isVectorValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
        CMultiTypeDataArrayWrapperSPtr nodes_array(data->getVectorValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
        for(int idx = 0; idx < nodes_array->size(); idx++) {
            associated_nodes.push_back(nodes_array->getStringElementAtIndex(idx));
            association_list.push_back(AgentAssociation(nodes_array->getStringElementAtIndex(idx), "", ""));
        }
    } else if(data->isStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
        association_list.push_back(AgentAssociation(data->getStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED), "", ""));
    } else {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("The key %1% need to be vector of string or string", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    }
    //! fetch the agent information
    CDataWrapper *tmp_ptr;
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(agent_uid, &tmp_ptr))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error fetching node decription for %1%", %agent_uid))
    }
    agent_node_information.reset(tmp_ptr);
    
    scan_phase = ScanPhaseNext;
    current_association_it = association_list.begin();
}

// inherited method
void AgentRemoveNodeSafety::acquireHandler() {
    
}

// inherited method
void AgentRemoveNodeSafety::ccHandler() {
    switch(scan_phase) {
        case ScanPhaseNext:{
            if(current_association_it == association_list.end()) {
                //we have terminated the work
                INFO << CHAOS_FORMAT("All associated node to remove for agent %1% has ben processed", %agent_uid);
                scan_phase = ScanPhaseEnd;
                break;
            }
            DBG << CHAOS_FORMAT("Process remove operation for node %1% associated to the agent %2%", %current_association_it->associated_node_uid%agent_uid);
            //compose the stop message with kill feature(we must force)
            message_data = composeStopMessage(*current_association_it);
            //create the request
            request = createRequest(agent_node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE);
            //at this point folow directly to the next case
            scan_phase = ScanPhaseProcess;
        }
        case ScanPhaseProcess:{
            if(processStopOperationPhases()) {
                //we need to process the next association
                current_association_it++;
                scan_phase = ScanPhaseNext;
            }
            break;
        }
        case ScanPhaseEnd: {
            //send an alerto to inform all listener that hase benn done a cross check
            //we can send the event
            if(alert_event_channel){
                //send broadcast event
                alert_event_channel->sendAgentProcessCheckAlert(agent_uid, 1);
            }
            BC_END_RUNNING_PROPERTY;
            break;
        }
    }
}

// inherited method
bool AgentRemoveNodeSafety::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}

CDWUniquePtr AgentRemoveNodeSafety::composeStopMessage(AgentAssociation& agent_association,
                                                                      bool kill) {
    CDWUniquePtr result(new CDataWrapper());
    AgentAssociationSDWrapper assoc_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, agent_association));
    result = assoc_sd_wrapper.serialize();
    result->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL,
                         kill);
    return result;
}

bool AgentRemoveNodeSafety::processStopOperationPhases() {
    int err = 0;
    bool got_to_next_scan_phase = 0;
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendRequest(*request,
                        MOVE(message_data));
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
            INFO << CHAOS_FORMAT("Associated node %1% for agent %2% will be stopped and removed", %current_association_it->associated_node_uid%agent_uid);
            if((err = getDataAccess<mds_data_access::AgentDataAccess>()->removeNodeAssociationForAgent(agent_uid, current_association_it->associated_node_uid))) {
                LOG_AND_TROW(ERR, -3, CHAOS_FORMAT("Error removing the node %1% association for agent %2% with error %3%",%current_association_it->associated_node_uid%agent_uid%err));
            }
            got_to_next_scan_phase = true;
            break;
    }
    return got_to_next_scan_phase;
}
