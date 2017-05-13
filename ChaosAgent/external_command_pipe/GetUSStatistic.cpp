/*
 *	GetUSStatistic.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/03/2017 INFN, National Institute of Nuclear Physics
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
    std::unique_ptr<CDataWrapper> api_data(new CDataWrapper());
    
    const std::string& agent_uid = ChaosAgent::getInstance()->settings.agent_uid;
    
    api_data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    api_data->addBoolValue("load_related_data", true);
    
    
    std::unique_ptr<MultiAddressMessageRequestFuture> request = mds_message_channel.sendRequestWithFuture(AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                                                                                        "loadAgentDescription",
                                                                                                        api_data.release(),
                                                                                                        5000);
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
