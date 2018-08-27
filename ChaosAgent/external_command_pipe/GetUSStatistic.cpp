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

#include "GetUSStatistic.h"
#include "../ChaosAgent.h"
#include "../utility/ProcUtil.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos_service_common/data/node/Agent.h>

using namespace chaos::common::data;
using namespace chaos::common::message;

using namespace chaos::service_common::data::agent;

using namespace chaos::agent::external_command_pipe;

GetUSStatistic::GetUSStatistic(AbstractCommandOutputStream& _cmd_ostream,
                               common::message::MDSMessageChannel& _mds_message_channel):
AbstractExternalCommand("US_STAT",
                        _cmd_ostream,
                        _mds_message_channel){}

GetUSStatistic::~GetUSStatistic(){}

int GetUSStatistic::execute(ChaosStringVector input_parameter) {
    if(input_parameter.size() != 1) {
        println("Invalid parameter number");
        return 1;
    }
    
    AgentInstanceSDWrapper agent_instance_sd_wrapper;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    
    const std::string& agent_uid = ChaosAgent::getInstance()->settings.agent_uid;
    
    api_data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    api_data->addBoolValue("load_related_data", true);
    
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request = mds_message_channel.sendRequestWithFuture(AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                                                                                        "loadAgentDescription",
                                                                                                        ChaosMoveOperator(api_data),
                                                                                                         RpcConfigurationKey::GlobalRPCTimeoutinMSec);
    if(request->wait() == false ||
       request->getError() != 0) {
        return 2;
    }
    
    //we have the description
    std::string tmp_out;
    agent_instance_sd_wrapper.deserialize(request->getResult());
    const VectorAgentAssociation& ass_vec = agent_instance_sd_wrapper().node_associated;
    if(ass_vec.size() > 0) {
        print("|");
        for(VectorAgentAssociationConstIterator it = ass_vec.begin(),
            end = ass_vec.end();
            it != end;
            it++) {
            bool alive = utility::ProcUtil::checkProcessAlive(*it);
            tmp_out += CHAOS_FORMAT("%1%|%2%|", %it->associated_node_uid%alive);
        }
        println(tmp_out);
    }
    return 0;
}
