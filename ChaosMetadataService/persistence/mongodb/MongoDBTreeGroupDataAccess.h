/*
 *	MongoDBTreeGroupDataAccess.h
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

#ifndef MongoDBTreeGroupDataAccess_hpp
#define MongoDBTreeGroupDataAccess_hpp

#include "../data_access/TreeGroupDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for manage the tree group
                /*!
                 In mongo db tree structure is done using a parent key within a tree child node. The stirng representation
                 of the full path is : /root/child1/child2/child3.... Every tree live in a domain that is puth into antoher
                 key of the single node.
                 0
                 node {
                    node_name:string,
                    node_parent_path:string
                    node_domain:string
                 }
                 
                 for describe the example :
                 node {
                 node_name:"child3",
                 node_parent_path:"/root/child1/child2"
                 node_domain:"domain_name"
                 }
                 */
                class MongoDBTreeGroupDataAccess:
                public data_access::TreeGroupDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    MongoDBTreeGroupDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBTreeGroupDataAccess();
                    
                    /*!
                     Check if a node is present.
                     */
                    int checkNodePresenceForDomain(const std::string& group_domain,
                                                   const std::string& node_name,
                                                   const std::string& tree_path,
                                                   bool& presence);
                    
                    /*!
                     Check if a path is presente, thech is done in this way; last path element is considered as
                     node name and the ancestor as parent path.
                     */
                    int checkPathPresenceForDomain(const std::string& group_domain,
                                                   const std::string& tree_path,
                                                   bool& presence);
                public:
                    
                    //! Inherited method
                    int deleteGroupDomain(const std::string& group_domain);
                    
                    //! Inherited method
                    int getAllGroupDomain(std::vector<std::string>& group_domain_list);
                    
                    //! Inherited method
                    int addNewNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& node_name,
                                                const std::string& parent_path);
                    //! inherited method
                    int addNewNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& node_group_name);
                    //! Inherited method
                    int deleteNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& tree_path);
                    //! Inherited method
                    int deleteNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& node_group_name,
                                                const std::string& parent_path);
                    //! Inherited method
                    int getNodeChildFromPath(const std::string& group_domain,
                                             const std::string& tree_path,
                                             std::vector<std::string>& node_child);
                    //! Inherited method
                    int getNodeRootFromDomain(const std::string& group_domain,
                                              std::vector<std::string>& node_child);
                    //! Inherited method
                    int addChaosNodeToGroupDomain(const std::string& group_domain,
                                                  const std::string& tree_path,
                                                  const std::string& chaos_node_type,
                                                  const std::string& chaos_node_uid);
                    
                    //! Inherited method
                    int removeChaosNodeFromGroupDomain(const std::string& group_domain,
                                                       const std::string& tree_path,
                                                       const std::string& chaos_node_type,
                                                       const std::string& chaos_node_uid);
                    
                    //! Inherited method
                    int getChaosNodeFromGroupDomain(const std::string& group_domain,
                                                    const std::string& tree_path,
                                                    data_access::TreeGroupChaosNodeList& chaos_node_list_in_group,
                                                    const unsigned int page_size,
                                                    const std::string& last_node_uid = std::string());
                };
            }
        }
    }
}

#endif /* MongoDBTreeGroupDataAccess_hpp */
