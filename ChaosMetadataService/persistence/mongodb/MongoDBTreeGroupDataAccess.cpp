/*
 *	MongoDBTreeGroupDataAccess.cpp
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

#include "MongoDBTreeGroupDataAccess.h"

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBTreeGroupDataAccess::MongoDBTreeGroupDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
TreeGroupDataAccess(){}

MongoDBTreeGroupDataAccess::~MongoDBTreeGroupDataAccess() {}

//! Inherited method
int MongoDBTreeGroupDataAccess::addNewGroupDomain(const std::string& group_domain) {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::deleteGroupDomain(const std::string& group_domain)  {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::addNewNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& node_group_name,
                                                        const std::string& parent_path) {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::deleteNodeGroupToDomain(const std::string& group_domain,
                                                        const std::string& tree_path) {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::addChaosNodeToGroupDomain(const std::string& group_domain,
                                                          const std::string& tree_path,
                                                          const std::string& chaos_node_type,
                                                          const std::string& chaos_node_uid) {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::removeChaosNodeFromGroupDomain(const std::string& group_domain,
                                                               const std::string& tree_path,
                                                               const std::string& chaos_node_type,
                                                               const std::string& chaos_node_uid) {
    return -1;
}

//! Inherited method
int MongoDBTreeGroupDataAccess::getChaosNodeFromGroupDomain(const std::string& group_domain,
                                                            const std::string& tree_path,
                                                            data_access::TreeGroupChaosNodeList& chaos_node_list_in_group,
                                                            const unsigned int page_size,
                                                            const std::string& last_node_uid) {
    return -1;
}