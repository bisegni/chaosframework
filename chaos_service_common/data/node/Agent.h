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
                
                //!describe th server layer managemento for angent data
                struct AgentManagementSetting {
                    //!identify the state of expiration of the agent log entry
                    bool expiration_enabled;
                    //! define the expiration time in seconds for every log entry
                    uint32_t log_expiration_in_seconds;
                    
                    AgentManagementSetting():
                    expiration_enabled(false),
                    log_expiration_in_seconds(0){}
                    
                    AgentManagementSetting(const AgentManagementSetting& copy_src):
                    expiration_enabled(copy_src.expiration_enabled),
                    log_expiration_in_seconds(copy_src.log_expiration_in_seconds){}
                    
                    AgentManagementSetting& operator=(AgentManagementSetting const &rhs) {
                        if(this == &rhs) return *this;
                        expiration_enabled = rhs.expiration_enabled;
                        log_expiration_in_seconds = rhs.log_expiration_in_seconds;
                        return *this;
                    }
                };
                
                //!sd wrapper for AgentManagementSetting
                CHAOS_OPEN_SDWRAPPER(AgentManagementSetting)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().expiration_enabled = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, "log_enable_state", false);
                    dataWrapped().log_expiration_in_seconds = CDW_GET_INT32_WITH_DEFAULT(serialized_data, "log_expiration", 0);
                    
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addBoolValue("log_enable_state", dataWrapped().expiration_enabled);
                    data_serialized->addInt32Value("log_expiration", dataWrapped().log_expiration_in_seconds);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                
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
                    //!qgent automatically respawn process it it will be killed
                    bool  keep_alive;
                    //!qgent automatically route log to mds for launched process
                    bool  log_at_laucnh;
                    
                    AgentAssociation():
                    associated_node_uid(),
                    configuration_file_content(),
                    auto_start(false),
                    keep_alive(false),
                    log_at_laucnh(false){}
                    
                    AgentAssociation(const std::string& _associated_node_uid,
                                     const std::string& _launch_cmd_line,
                                     const std::string& _configuration_file_content,
                                     const bool _auto_start = false,
                                     const bool _keep_alive = false,
                                     const bool _log_at_launch = false):
                    association_unique_id(),
                    associated_node_uid(_associated_node_uid),
                    launch_cmd_line(_launch_cmd_line),
                    configuration_file_content(_configuration_file_content),
                    auto_start(_auto_start),
                    keep_alive(_keep_alive),
                    log_at_laucnh(_log_at_launch){}
                    
                    AgentAssociation(const AgentAssociation& copy_src):
                    association_unique_id(copy_src.association_unique_id),
                    associated_node_uid(copy_src.associated_node_uid),
                    launch_cmd_line(copy_src.launch_cmd_line),
                    configuration_file_content(copy_src.configuration_file_content),
                    auto_start(copy_src.auto_start),
                    keep_alive(copy_src.keep_alive),
                    log_at_laucnh(copy_src.log_at_laucnh){}
                    
                    AgentAssociation& operator=(AgentAssociation const &rhs) {
                        if(this == &rhs) return *this;
                        association_unique_id = rhs.association_unique_id;
                        associated_node_uid = rhs.associated_node_uid;
                        launch_cmd_line = rhs.launch_cmd_line;
                        configuration_file_content = rhs.configuration_file_content;
                        auto_start = rhs.auto_start;
                        keep_alive = rhs.keep_alive;
                        log_at_laucnh = rhs.log_at_laucnh;
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
                    dataWrapped().keep_alive = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_KEEP_ALIVE, false);
                    dataWrapped().log_at_laucnh = CDW_GET_BOOL_WITH_DEFAULT(serialized_data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_LOG_AT_LAUNCH, false);
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addStringValue("association_uid", dataWrapped().association_unique_id);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME, dataWrapped().associated_node_uid);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_CMD_LINE, dataWrapped().launch_cmd_line);
                    data_serialized->addStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG, dataWrapped().configuration_file_content);
                    data_serialized->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_AUTO_START, dataWrapped().auto_start);
                    data_serialized->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_KEEP_ALIVE, dataWrapped().keep_alive);
                    data_serialized->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_LOG_AT_LAUNCH, dataWrapped().log_at_laucnh);
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
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
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
                    node::NodeInstanceSDWrapper node_instance(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(node::NodeInstance, dataWrapped()));
                    node_instance.deserialize(serialized_data);
                    dataWrapped().working_directory = CDW_GET_SRT_WITH_DEFAULT(serialized_data, AgentNodeDefinitionKey::WORKING_DIRECTORY, "");
                    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, dataWrapped().node_associated));
                    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
                    associationList_sd_wrap.deserialize(serialized_data);
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    node::NodeInstanceSDWrapper node_instance(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(node::NodeInstance, dataWrapped()));
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized = node_instance.serialize();
                    data_serialized->addStringValue(AgentNodeDefinitionKey::WORKING_DIRECTORY, dataWrapped().working_directory);
                    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, dataWrapped().node_associated));
                    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> ser_vec = associationList_sd_wrap.serialize();
                    data_serialized->appendAllElement(*ser_vec);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                //!identify a log entry grabbed by an agen from a node
                struct AgentLogEntry {
                    uint64_t entry_ts;
                    uint64_t entry_seq;
                    std::string entry_log_line;
                };
                
                CHAOS_OPEN_SDWRAPPER(AgentLogEntry)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().entry_ts = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "log_ts", 0);
                    dataWrapped().entry_seq = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "log_seq", 0);
                    dataWrapped().entry_log_line = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "log_entry", "");
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addInt64Value("log_ts", dataWrapped().entry_ts);
                    data_serialized->addInt64Value("log_seq", dataWrapped().entry_seq);
                    data_serialized->addStringValue("log_entry", dataWrapped().entry_log_line);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(AgentLogEntry, VectorAgentLogEntry);
                typedef chaos::common::data::structured::StdVectorSDWrapper<AgentLogEntry, AgentLogEntrySDWrapper> VectorAgentLogEntrySDWrapper;
            }
        }
    }
}

#endif /* __CHAOSFramework__6033D74_69EF_4071_A3D3_43805A319A13_agent_h */
