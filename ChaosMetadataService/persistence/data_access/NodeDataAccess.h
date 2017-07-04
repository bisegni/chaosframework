/*
 *	NodeDataAccess.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__NodeDataAccess__
#define __CHAOSFramework__NodeDataAccess__

#include "../persistence.h"

#include <chaos/common/data/CDataWrapper.h>
#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                /*!
                 Performs the basic operation on abstract node infomation
                 */
                class NodeDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                public:
                    DECLARE_DA_NAME
                    //! default constructor
                    NodeDataAccess();
                    
                    //! defautl destructor
                    ~NodeDataAccess();
                    
                    //! returns the whole node description
                    /*!
                     returns the whole node description as is on database
                     \param node_unique_id the unique id of the node
                     \param node_description the handler to the node description
                     */
                    virtual int getNodeDescription(const std::string& node_unique_id,
                                                   chaos::common::data::CDataWrapper **node_description) = 0;
                    
                    //! inserts a new node
                    /*!
                     The API receive a data pack with one or more key listed in chaos::NodeDefinitionKey namespace,
                     the mandatory key are:
                     NODE_UNIQUE_ID,
                     NODE_TYPE [values need to be one of the @chaos::NodeType],
                     NODE_RPC_ADDR,
                     NODE_RPC_DOMAIN,
                     NODE_TIMESTAMP.
                     \param node_description the node description
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int insertNewNode(chaos::common::data::CDataWrapper& node_description) = 0;
                    
                    //! updates the node information
                    /*!
                     The API receive a data pack with one or more key listed in chaos::NodeDefinitionKey namespace.
                     The mandatory key are the NODE_UNIQUE_ID need to find the description and the only updateable key are:
                     NODE_RPC_ADDR,
                     NODE_RPC_DOMAIN,
                     NODE_TIMESTAMP.
                     The NODE_TYPE key can't be updated
                     \param node_description the node description
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int updateNode(chaos::common::data::CDataWrapper& node_description) = 0;
                    
                    
                    //! checks if a unit server identified by unique id is preset
                    /*!
                     The API check is the description of the node has been created. The variable represent the answer to the
                     request only if the result of the api is 0;
                     \param unit_server_alias unit server unique id
                     \param presence = true if the unit server is present
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int checkNodePresence(bool& presence,
                                                  const std::string& node_unique_id,
                                                  const std::string& node_unique_type = std::string()) = 0;
                    
                    //! sets node health information
                    /*!
                     Set and update the health information for a node.
                     \param node_unique_id is the unique id of the node that has sent the health information
                     \param health_stat is the helath stat information
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int setNodeHealthStatus(const std::string& node_unique_id,
                                                    const common::data::structured::HealthStat& health_stat) = 0;
                    
                    //! gets node health information
                    /*!
                     get the health information for a node.
                     \param node_unique_id is the unique id of the node that has sent the health information
                     \param health_stat is the helath stat information
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int getNodeHealthStatus(const std::string& node_unique_id,
                                                    common::data::structured::HealthStat& health_stat) = 0;
                    
                    //! deletes the node description
                    /*
                     The API delete the node description.
                     */
                    virtual int deleteNode(const std::string& node_unique_id,
                                           const std::string& node_type = std::string()) = 0;
                    
                    //!Makes simple paged query on node
                    /*!
                     perform a simple search on node filtering on type
                     \param result the handle for the reuslt data (is allcoated only if the search has been done with success)
                     \param criteria the string that filter the node (susing * semantic)
                     \param if true return only node that are actualy alive
                     \param search_type the type of the nodes where search
                     \param last_unique_id is the start of the search page to retrieve
                     */
                    virtual int searchNode(chaos::common::data::CDataWrapper **result,
                                           const std::string& criteria,
                                           uint32_t search_type,
                                           bool alive_only,
                                           uint32_t last_unique_id,
                                           uint32_t page_length = 100) = 0;
                    
                    //! Checks a command presence
                    /*!
                     \param command_unique_id is the unique identifir ther represent the command
                     \param presence true if the template exists false otherwhise
                     */
                    virtual int checkCommandPresence(const std::string& command_unique_id,
                                                     bool& presence) = 0;
                    
                    //! sets (insert or update) command description
                    /*!
                     \param command the description
                     */
                    virtual int setCommand(chaos::common::data::CDataWrapper& command) = 0;
                    
                    //! returns a command
                    /*!
                     \param comamnd_uid the unique identifier of the command
                     \param command the description
                     */
                    virtual int getCommand(const std::string& command_unique_id,
                                           chaos::common::data::CDataWrapper **command) = 0;
                    
                    //! deletes a command description
                    /*!
                     \param the uid of the command to delete
                     */
                    virtual int deleteCommand(const std::string& command_unique_id) = 0;
                    
                    //! sets (insert or update) a template for a batch command
                    /*!
                     The CDataWrapper contains all the key show on SetCommandTemplate api documentation
                     */
                    virtual int setCommandTemplate(chaos::common::data::CDataWrapper& command_template) = 0;
                    
                    //! Checks a command template presence
                    /*!
                     Try to find a command template
                     \param template_name is the name that semantically represent the template
                     \param command_unique_id is the unique identifir ther represent the command
                     \param presence true if the template exists false otherwhise
                     */
                    virtual int checkCommandTemplatePresence(const std::string& template_name,
                                                             const std::string& command_unique_id,
                                                             bool& presence) = 0;
                    //! Deletes a command template
                    /*!
                     Delete a template for a batch command
                     \param template_name is the name that semantically represent the template
                     \param command_unique_id is the unique identifir ther represent the command
                     */
                    virtual int deleteCommandTemplate(const std::string& template_name,
                                                      const std::string& command_unique_id) = 0;
                    
                    //! Returns a command template
                    /*!
                     search and if is present return the template by name and command uid
                     \param template_name is the name that semantically represent the template
                     \param command_unique_id is the unique identifir ther represent the command
                     */
                    virtual int getCommandTemplate(const std::string& template_name,
                                                   const std::string& command_unique_id,
                                                   chaos::common::data::CDataWrapper **command_template) = 0;
                    
                    //!Makes simple command template query
                    /*!
                     perform a simple search on node command template
                     \param result the handle for the reuslt data (is allcoated only if the search has been done with success)
                     \param cmd_uid_to_filter the list of the command unique id for filtering operation
                     \param last_unique_id is the start of the search page to retrieve
                     */
                    virtual int searchCommandTemplate(chaos::common::data::CDataWrapper **result,
                                                      const std::vector<std::string>& cmd_uid_to_filter,
                                                      uint32_t last_unique_id,
                                                      uint32_t page_length = 100) = 0;
                    
                    //!adds data structure to node for permit ageing management
                    virtual int addAgeingManagementDataToNode(const std::string& control_unit_id) = 0;
                    
                    //                    virtual int reserveNodeForAgeingManagement(uint64_t& last_sequence_id,
                    //                                                               std::string& node_uid_reserved,
                    //                                                               uint32_t& node_ageing_time,
                    //                                                               uint64_t& last_ageing_perform_time,
                    //                                                               uint64_t timeout_for_checking,
                    //                                                               uint64_t delay_next_check) = 0;
                    
                    //                    virtual int releaseNodeForAgeingManagement(std::string& node_uid,
                    //                                                                      bool performed) = 0;
                    
                    //!checks if a node is alive
                    /*!
                     using helat data store on database check if the node i still alive
                     \param node_uid is the node for wich we want to check the alive state
                     \param alive contai the state of the node if function doesn't report error
                     */
                    virtual int isNodeAlive(const std::string& node_uid, bool& alive) = 0;

                    //!Adds a set of tags to a specific node
                    /*!
                     \param node_uid node identifier
                     \param tags a set of tags to be applied to the specific node
                     */
                    virtual int setTags(const std::string& node_uid, ChaosStringSet& tags) = 0;

                    //!Removes a set of tags from a specific node
                    /*!
                     \param node_uid node identifier
                     \param tags a set of tags to be removed from the specific node
                     */
                    virtual int dropTags(const std::string& node_uid, ChaosStringSet& tags) = 0;

                    //!Removes all tags from a specific node
                    /*!
                     \param node_uid node identifier
                     */
                    virtual int dropAllTags(const std::string& node_uid) = 0;

                    //!Returns all the tags assigned to a specific node
                    /*!
                     \param node_uid node identifier
                     \param tags a set containing all the tags assigned to the specific node
                     */
                    virtual int listTags(const std::string& node_uid, ChaosStringSet& tags) = 0;
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
