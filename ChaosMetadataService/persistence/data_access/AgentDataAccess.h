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

#ifndef __CHAOSFramework__DA13C6A_2DD1_4295_A51A_AB24EFFA60D8_AgentDataAccess_h
#define __CHAOSFramework__DA13C6A_2DD1_4295_A51A_AB24EFFA60D8_AgentDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>
#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                //!agent data managment
                class AgentDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                    
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    AgentDataAccess();
                    
                    //!default destructor
                    ~AgentDataAccess();
                    
                    //! Insert or update an agent descirption
                    virtual int insertUpdateAgentDescription(chaos::common::data::CDataWrapper& agent_description) = 0;
                    
                    virtual int loadAgentDescription(const std::string& agent_uid,
                                                     const bool load_related_data,
                                                     chaos::service_common::data::agent::AgentInstance& agent_description) = 0;
                    
                    //! return, if found, the uid of the agent that has the associated node
                    virtual int getAgentForNode(const std::string& associated_node_uid,
                                                std::string& agent_uid) = 0;
                    
                    //! save(if not preset inser or update) the node association description
                    virtual int saveNodeAssociationForAgent(const std::string& agent_uid,
                                                            chaos::service_common::data::agent::AgentAssociation& node_association) = 0;
                    //! load the node association description
                    virtual int loadNodeAssociationForAgent(const std::string& agent_uid,
                                                            const std::string& associated_node_uid,
                                                            chaos::service_common::data::agent::AgentAssociation& node_association) = 0;
                    
                    //! remove the node association description
                    virtual int removeNodeAssociationForAgent(const std::string& agent_uid,
                                                              const std::string& associated_node_uid) = 0;
                    
                    virtual int setNodeAssociationStatus(const std::string& agent_uid,
                                                         const chaos::service_common::data::agent::AgentAssociationStatus& status) = 0;
                    
                    //! return all unit server associated with agent
                    virtual int getNodeListStatusForAgent(const std::string& agent_uid,
                                                          chaos::service_common::data::agent::VectorAgentAssociationStatus& node_status_vec) = 0;
                    
                    //! push a log entry received from angent process log system
                    virtual int pushLogEntry(const std::string& node_uid,
                                             const std::string& node_log_entry) = 0;
                    
                    //! serach on log entries
                    virtual int getLogEntry(const std::string& node_uid,
                                            const int32_t number_of_entries,
                                            const bool asc,
                                            const uint64_t start_seq,
                                            chaos::service_common::data::agent::VectorAgentLogEntry& found_entries) = 0;
                    
                    virtual int setLogEntryExpiration(const bool expiration_active,
                                                      const uint32_t expiration_in_seconds) = 0;
                    
                    virtual int getLogEntryExpiration(bool& expiration_active,
                                                      uint32_t& expiration_in_seconds) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__DA13C6A_2DD1_4295_A51A_AB24EFFA60D8_AgentDataAccess_h */
