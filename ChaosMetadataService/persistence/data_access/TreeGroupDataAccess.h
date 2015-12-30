/*
 *	TreeGroupDataAccess.h
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

#ifndef __CHAOSFramework__TreeGroupDataAccess_h
#define __CHAOSFramework__TreeGroupDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                struct ChaosNodeInGroupElement {
                    std::string node_uid;
                    std::string node_type;
                    ChaosNodeInGroupElement(const std::string& _node_uid,
                                            const std::string& _node_type):
                    node_uid(_node_uid),
                    node_type(_node_type){}
                    
                    ChaosNodeInGroupElement(const ChaosNodeInGroupElement& _origin):
                    node_uid(_origin.node_uid),
                    node_type(_origin.node_type){}
                };
                
                //define the list of chaos node contained is a group
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosNodeInGroupElement, TreeGroupChaosNodeList)
                
                class TreeGroupDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                protected:
                    
                    //!check the three path sintax
                    bool checkPathSintax(const std::string& tree_path);
                    
                    //! check the node name sintax
                    bool checkNodeNameSintax(const std::string& node_name);
                    
                    //!extract the node name and parent path from the global path to a node
                    bool estractNodeFromPath(const std::string& node_path,
                                             std::string& node_name,
                                             std::string& parent_path);
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    TreeGroupDataAccess();
                    
                    //!default destructor
                    ~TreeGroupDataAccess();
                    
                    //! remove an intere domain of groups
                    /*!
                     Remove the odmain and all contained tree
                     \param group_domain is the domain of the group
                     */
                    virtual int deleteGroupDomain(const std::string& group_domain) = 0;
                    
                    //! return all domain of the groups
                    /*!
                     \param group_domain_list is filled with all domain found
                     */
                    virtual int getAllGroupDomain(std::vector<std::string>& group_domain_list) = 0;
                    
                    //! Add a new node in a tree group
                    /*!
                     Add a new node in the tree group
                     \param group_domain is the domain of the node
                     \param node_group_name is the name of the new node in the tree
                     \param parent_path is the name of the path of the parent, if parent is null a new root is created
                     */
                    virtual int addNewNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name,
                                                        const std::string& parent_path) = 0;
                    
                    //! Add a new root node in a tree group
                    /*!
                     Add a new node in the tree group
                     \param group_domain is the domain of the node
                     \param node_group_name is the name of the new node in the tree
                     */
                    virtual int addNewNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name) = 0;
                    
                    //! Delete a node in a tree group
                    /*!
                     Delete a node in the tree group using an absolute path within domain, all child will be deleted
                     \param group_domain is the domain of the node
                     \param tree_path is the name of the path to the node to delete
                     */
                    virtual int deleteNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& tree_path) = 0;
                    
                    //! Delete a node in a tree group
                    /*!
                     Delete a node in the tree group using an node name, all child will be deleted
                     \param group_domain is the domain of the node
                     \param node_group_name is the name of the node to delete in the tree in the tree
                     \param parent_path is the name of the path of the parent, if parent is null it will be a root node
                     */
                    virtual int deleteNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name,
                                                        const std::string& parent_path) = 0;
                    
                    //!return the list of node child for the node represented by path
                    /*!
                     \param group_domain is the domain of the node
                     \param tree_path is the name of the path to the node used for find the cild
                     */
                    virtual int getNodeChildFromPath(const std::string& group_domain,
                                                     const std::string& tree_path,
                                                     std::vector<std::string>& node_child) = 0;
                    
                    //!return the list of root node for a domain
                    /*!
                     \param group_domain is the domain of the node
                     */
                    virtual int getNodeRootFromDomain(const std::string& group_domain,
                                                      std::vector<std::string>& node_child) = 0;
                    
                    //! Add an entity to a group in a domain
                    /*!
                     The implementation connect the chaos node to the specified group in the domain
                     \param group_domain is the domain of the node
                     \param tree_path is the name of the path of the parent, if parent is null a new root is created
                     \param chaos_node_type is chaos node to attacch to the group
                     \param chaos_node_uid is the unique id of the chaos node
                     */
                    virtual int addChaosNodeToGroupDomain(const std::string& group_domain,
                                                          const std::string& tree_path,
                                                          const std::string& chaos_node_type,
                                                          const std::string& chaos_node_uid) = 0;
                    
                    //! remove an entity from a group in a domain
                    /*!
                     The implementation connect the chaos node to the specified group in the domain
                     \param group_domain is the domain of the node
                     \param tree_path is the name of the path of the parent, if parent is null a new root is created
                     \param chaos_node_type is chaos node to attacch to the group
                     \param chaos_node_uid is the unique id of the chaos node
                     */
                    virtual int removeChaosNodeFromGroupDomain(const std::string& group_domain,
                                                               const std::string& tree_path,
                                                               const std::string& chaos_node_type,
                                                               const std::string& chaos_node_uid) = 0;
                    
                    //! Get all the chaos node form the group
                    /*!
                     The implementation connect the chaos node to the specified group in the domain
                     \param group_domain is the domain of the node
                     \param tree_path is the name of the path of the parent, if parent is null a new root is created
                     \param chaos_node_list_in_group is the node
                     \param last_node_uid is the uid of the last node(used for paging
                     \param page_size is the size of the current returned result page
                     */
                    virtual int getChaosNodeFromGroupDomain(const std::string& group_domain,
                                                            const std::string& tree_path,
                                                            TreeGroupChaosNodeList& chaos_node_list_in_group,
                                                            const unsigned int page_size,
                                                            const std::string& last_node_uid = std::string()) = 0;
                };
            }
        }
    }
}


#endif /* TreeGroupDataAccess_h */
