/*
 *	agent.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__6033D74_69EF_4071_A3D3_43805A319A13_agent_h
#define __CHAOSFramework__6033D74_69EF_4071_A3D3_43805A319A13_agent_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/structured/Lists.h>
#include <chaos_service_common/data/node/Node.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace agent {
                
                typedef enum {
                    NodeAssociationOperationUndefined,
                    NodeAssociationOperationLaunch,
                    NodeAssociationOperationStop,
                    NodeAssociationOperationKill,
                    NodeAssociationOperationRestart
                } NodeAssociationOperation;
                
                typedef enum {
                    NodeAssociationLoggingOperationUndefined,
                    NodeAssociationLoggingOperationDisable,
                    NodeAssociationLoggingOperationEnable
                } NodeAssociationLoggingOperation;
                
                //!node association to an agent
                struct AgentAssociation {
                    //!identify the node of node associated to agent
                    std::string  association_unique_id;
                    //!identify the node of node associated to agent
                    std::string  associated_node_uid;
                    //!string to pass to the system for launch the process
                    std::string  launch_cmd_line;
                    //!contains the configuration file
                    std::string  configuration_file_content;
                    //!qgent start automatically this node(also after crash)
                    bool  auto_start;
                    
                    AgentAssociation():
                    associated_node_uid(),
                    configuration_file_content(),
                    auto_start(false){}
                    
                    AgentAssociation(const std::string& _associated_node_uid,
                                     const std::string& _launch_cmd_line,
                                     const std::string& _configuration_file_content,
                                     const bool _auto_start):
                    association_unique_id(),
                    associated_node_uid(_associated_node_uid),
                    launch_cmd_line(_launch_cmd_line),
                    configuration_file_content(_configuration_file_content),
                    auto_start(_auto_start){}
                    
                    AgentAssociation(const AgentAssociation& copy_src):
                    association_unique_id(copy_src.association_unique_id),
                    associated_node_uid(copy_src.associated_node_uid),
                    launch_cmd_line(copy_src.launch_cmd_line),
                    configuration_file_content(copy_src.configuration_file_content),
                    auto_start(copy_src.auto_start){}
                    
                    AgentAssociation& operator=(AgentAssociation const &rhs) {
                        association_unique_id = rhs.association_unique_id;
                        associated_node_uid = rhs.associated_node_uid;
                        launch_cmd_line = rhs.launch_cmd_line;
                        configuration_file_content = rhs.configuration_file_content;
                        auto_start = rhs.auto_start;
                        return *this;
                    }
                };
                
                //!sd wrapper for agent associateion class
                CHAOS_OPEN_SDWRAPPER(AgentAssociation)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().association_unique_id = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "association_uid", "");
                    dataWrapped().associated_node_uid = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME, "");
                    dataWrapped().launch_cmd_line = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_CMD_LINE, "");
                    dataWrapped().configuration_file_content = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG, "");
                    dataWrapped().auto_start = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_AUTO_START, false);
                }
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                    std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addStringValue("association_uid", dataWrapped().association_unique_id);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME, dataWrapped().associated_node_uid);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_CMD_LINE, dataWrapped().launch_cmd_line);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG, dataWrapped().configuration_file_content);
                    data_serialized->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_AUTO_START, dataWrapped().auto_start);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                
                //!
                struct AgentAssociationStatus {
                    //!associated node uid
                    std::string     associated_node_uid;
                    //!alive state
                    bool            alive;
                    //!checking timestamp
                    uint64_t        check_ts;
                    
                    AgentAssociationStatus():
                    alive(false),
                    check_ts(0){}
                    
                    AgentAssociationStatus(const std::string& _associated_node_uid):
                    associated_node_uid(_associated_node_uid),
                    check_ts(0){}
                    
                    AgentAssociationStatus(const AgentAssociationStatus& copy_src):
                    associated_node_uid(copy_src.associated_node_uid),
                    alive(copy_src.alive),
                    check_ts(copy_src.check_ts){}
                    
                    AgentAssociationStatus& operator=(AgentAssociationStatus const &rhs) {
                        associated_node_uid = rhs.associated_node_uid;
                        alive = rhs.alive;
                        check_ts = rhs.check_ts;
                        return *this;
                    }
                };
                
                //!sd wrapper for agent association status class
                CHAOS_OPEN_SDWRAPPER(AgentAssociationStatus)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().associated_node_uid = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME, "");
                    dataWrapped().alive = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, "alive", false);
                    dataWrapped().check_ts = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "check_ts", 0);
                }
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                    std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME, dataWrapped().associated_node_uid);
                    data_serialized->addBoolValue("alive", dataWrapped().alive);
                    data_serialized->addInt64Value("check_ts", dataWrapped().check_ts);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                
                //!define the vector for association classes
                CHAOS_DEFINE_VECTOR_FOR_TYPE(AgentAssociation, VectorAgentAssociation);
                CHAOS_DEFINE_VECTOR_FOR_TYPE(AgentAssociationStatus, VectorAgentAssociationStatus);
                typedef chaos::common::data::structured::StdVectorSDWrapper<AgentAssociation, AgentAssociationSDWrapper> VectorAgentAssociationSDWrapper;
                typedef chaos::common::data::structured::StdVectorSDWrapper<AgentAssociationStatus, AgentAssociationStatusSDWrapper> VectorAgentAssociationStatusSDWrapper;
                
                //! base node instance description
                struct AgentInstance:
                public node::NodeInstance {
                    //!working directory of the agent
                    std::string working_directory;
                    //!associated node
                    VectorAgentAssociation node_associated;
                    
                    AgentInstance():
                    NodeInstance(){}
                    
                    AgentInstance(uint64_t _instance_seq,
                                 const std::string& _instance_name):
                    NodeInstance(_instance_seq,
                                 _instance_name){}
                    
                    AgentInstance(const AgentInstance& copy_src):
                    NodeInstance(copy_src),
                    working_directory(copy_src.working_directory){}
                    
                    AgentInstance& operator=(AgentInstance const &rhs) {
                        working_directory = rhs.working_directory;
                        ((NodeInstance*)this)->operator=(rhs);
                        return *this;
                    }
                };
                
                //!sd wrapper for agent associateion class
                CHAOS_OPEN_SDWRAPPER(AgentInstance)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().instance_seq = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "seq", 0);
                    dataWrapped().instance_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeDefinitionKey::NODE_UNIQUE_ID, "");
                    dataWrapped().working_directory = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDefinitionKey::WORKING_DIRECTORY, "");
                    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, dataWrapped().node_associated));
                    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
                    associationList_sd_wrap.deserialize(serialized_data);
                }
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                    std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addInt64Value("seq", dataWrapped().instance_seq);
                    data_serialized->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, dataWrapped().instance_name);
                    data_serialized->addStringValue(AgentNodeDefinitionKey::WORKING_DIRECTORY, dataWrapped().working_directory);
                    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, dataWrapped().node_associated));
                    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
                    std::auto_ptr<chaos::common::data::CDataWrapper> ser_vec = associationList_sd_wrap.serialize();
                    data_serialized->appendAllElement(*ser_vec);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__6033D74_69EF_4071_A3D3_43805A319A13_agent_h */
