/*
 *	AgentRegister.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/02/2017 INFN, National Institute of Nuclear Physics
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

#include "AgentRegister.h"
#include "AbstractAgent.h"
#include "impl/ProcessAgent.h"

#include <chaos/common/global.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#define SM_EXECTION_STEP_MS 1000

#define INFO    INFO_LOG(AgentRegister)
#define ERROR   ERR_LOG(AgentRegister)
#define DEBUG   DBG_LOG(AgentRegister)

using namespace chaos::agent;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;
using namespace chaos::common::async_central;

AgentRegister::AgentRegister():
agent_uid(CHAOS_FORMAT("ChaosAgent_%1%",%chaos::GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort())),
rpc_domain("agent"),
registration_state(AgentRegisterStateUnregistered),
mds_message_channel(NULL),
reg_retry_counter(0),
max_reg_retry_counter(5){}

AgentRegister::~AgentRegister() {}

void AgentRegister::addAgent(AgentSharedPtr new_agent) {
    if(new_agent.get() == NULL) return;
    if(map_agent.count(new_agent->getAgentName()) != 0) return;
    //add agent to map
    map_agent.insert(MapAgentPair(new_agent->getAgentName(), new_agent));
}

void AgentRegister::init(void *init_data) throw (chaos::CException) {
    //!register RPC action
    DeclareAction::addActionDescritionInstance<AgentRegister>(this,
                                                              &AgentRegister::registrationACK,
                                                              AgentNodeDomainAndActionRPC::RPC_DOMAIN,
                                                              AgentNodeDomainAndActionRPC::ACTION_AGENT_REGISTRATION_ACK,
                                                              "Execute the ack for agent registration message");
    //add all agent
    addAgent(AgentSharedPtr(new impl::ProcessAgent()));
    
    //!get metadata message channel
    mds_message_channel = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    CHECK_ASSERTION_THROW_AND_LOG((mds_message_channel != NULL), ERROR, -1, "Error creating new mds channel");
}

void AgentRegister::start() throw (chaos::CException) {
    //register rpc action 
    NetworkBroker::getInstance()->registerAction(this);
    //start the registering state machine
    registration_state = AgentRegisterStateStartRegistering;
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 0,
                                                 SM_EXECTION_STEP_MS);
}

void AgentRegister::stop() throw (chaos::CException) {
    //register rpc action
    NetworkBroker::getInstance()->deregisterAction(this);
    
    registration_state = AgentRegisterStateStartUnregistering;
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 0,
                                                 SM_EXECTION_STEP_MS);
    while(registration_state != AgentRegisterStateUnregistered &&
          registration_state != AgentRegisterStateFault) {
        usleep(500000);
    }
    
}

void AgentRegister::deinit() throw (chaos::CException) {
    
}

CDataWrapper* AgentRegister::registrationACK(CDataWrapper  *ack_pack,
                                             bool& detach) {
    CHECK_CDW_THROW_AND_LOG(ack_pack, ERROR, -1, CHAOS_FORMAT("ACK message with no contento for agent %1%", %agent_uid));
    CHECK_KEY_THROW_AND_LOG(ack_pack, NodeDefinitionKey::NODE_UNIQUE_ID, ERROR, -2, CHAOS_FORMAT("No identification of the device contained into the ack message for agent %1%", %agent_uid));
    CHECK_ASSERTION_THROW_AND_LOG((ack_pack->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID).compare(agent_uid) == 0), ERROR, -3, CHAOS_FORMAT("ACK message received by agent %1% was for a different agent %2% ", %agent_uid%ack_pack->getStringValue( NodeDefinitionKey::NODE_UNIQUE_ID)));
    if(ack_pack->hasKey(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT)) {
        int ack_val=ack_pack->getInt32Value(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT);
        switch(ack_val){
            case ErrorCode::EC_MDS_NODE_REGISTRATION_OK:
                registration_state = AgentRegisterStateRegistered;
                INFO << CHAOS_FORMAT("Agent %1% has been registered", %agent_uid);
                break;
                
            default:
                INFO << CHAOS_FORMAT("Agent %1% can be run by registration denied", %agent_uid);
                registration_state = AgentRegisterStateUnregistered;
                break;
        }
    }
    return NULL;
}


std::auto_ptr<CDataWrapper> AgentRegister::getAgentRegistrationPack() {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    if(result.get() == NULL) return result;
    result->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                           agent_uid);
    result->addStringValue(chaos::NodeDefinitionKey::NODE_TYPE,
                           chaos::NodeType::NODE_TYPE_AGENT);
    result->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR,
                           chaos::GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort());
    result->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN,
                           rpc_domain);
    result->addInt64Value(NodeDefinitionKey::NODE_TIMESTAMP,
                          TimingUtil::getTimeStamp());
    //add control unit alias
    for(MapAgentIterator iter = map_agent.begin();
        iter != map_agent.end();
        iter++) {
        std::auto_ptr<CDataWrapper> worker_definition(new CDataWrapper());
        CHAOS_ASSERT(worker_definition.get());
        //add worker name
        worker_definition->addStringValue(AgentNodeDefinitionKey::WORKER_NAME,
                                          iter->first);
        //get worker action
        iter->second->getActionDescrionsInDataWrapper(*worker_definition);
        
        result->appendCDataWrapperToArray(*worker_definition);
    }
    result->finalizeArrayForKey(AgentNodeDefinitionKey::HOSTED_WORKER);
    return result;
}

void AgentRegister::timeout() {
    switch(registration_state) {
        case AgentRegisterStateUnregistered:
            HealtManager::getInstance()->addNodeMetricValue(agent_uid,
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOAD);
            AsyncCentralManager::getInstance()->removeTimer(this);
            HealtManager::getInstance()->removeNode(agent_uid);
            break;
        case AgentRegisterStateStartRegistering: {
            reg_retry_counter = 0;
            //add healt metric for newly create control unit instance
            HealtManager::getInstance()->addNewNode(agent_uid);
            registration_state = AgentRegisterStateRegistering;
        }
        case AgentRegisterStateRegistering: {
            //send the rigstration pack
            if(((reg_retry_counter++)%max_reg_retry_counter) == 0) {
                std::auto_ptr<CDataWrapper> reg = getAgentRegistrationPack();
                mds_message_channel->sendNodeRegistration(*reg);
                HealtManager::getInstance()->addNewNode(agent_uid);
                HealtManager::getInstance()->addNodeMetricValue(agent_uid,
                                                                NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                                NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOADING);
            }
            break;
        }
        case AgentRegisterStateRegistered: {
            HealtManager::getInstance()->addNodeMetricValue(agent_uid,
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD);
            //stop timer
            AsyncCentralManager::getInstance()->removeTimer(this);
            
            //register all action
            try{
                for(MapAgentIterator iter = map_agent.begin();
                    iter != map_agent.end();
                    iter++) {
                    NetworkBroker::getInstance()->registerAction(iter->second.get());
                }
            }catch(chaos::CException& ex) {
                registration_state = AgentRegisterStateFault;
            }
            break;
        }
        case AgentRegisterStateStartUnregistering: {
            HealtManager::getInstance()->addNodeMetricValue(agent_uid,
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOADING);
            registration_state = AgentRegisterStateUnregistering;
        }
        case AgentRegisterStateUnregistering:
            reg_retry_counter = 0;
            registration_state = AgentRegisterStateUnregistered;
            for(MapAgentIterator iter = map_agent.begin();
                iter != map_agent.end();
                iter++) {
                NetworkBroker::getInstance()->deregisterAction(iter->second.get());
            }
            registration_state = AgentRegisterStateUnregistered;
            break;
            
        case AgentRegisterStateFault:
            HealtManager::getInstance()->addNodeMetricValue(agent_uid,
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR);
            break;
    }
    HealtManager::getInstance()->publishNodeHealt(agent_uid);
}
