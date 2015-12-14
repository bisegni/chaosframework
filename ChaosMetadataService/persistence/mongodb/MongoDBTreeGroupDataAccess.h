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
                class MongoDBTreeGroupDataAccess:
                public data_access::TreeGroupDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    MongoDBTreeGroupDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBTreeGroupDataAccess();
                public:
                    //! Inherited method
                    int addNewGroupDomain(const std::string& group_domain);
                    
                    //! Inherited method
                    int deleteGroupDomain(const std::string& group_domain);
                    
                    //! Inherited method
                    int addNewNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& node_group_name,
                                                const std::string& parent_path);
                    
                    //! Inherited method
                    int deleteNodeGroupToDomain(const std::string& group_domain,
                                                const std::string& tree_path);
                    
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
