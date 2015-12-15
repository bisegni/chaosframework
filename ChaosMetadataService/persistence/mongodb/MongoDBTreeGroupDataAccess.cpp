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

#include "mongo_db_constants.h"
#include "MongoDBTreeGroupDataAccess.h"

#include <mongo/client/dbclient.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#define MDBTGDA_INFO INFO_LOG(MongoDBTreeGroupDataAccess)
#define MDBTGDA_DBG  DBG_LOG(MongoDBTreeGroupDataAccess)
#define MDBTGDA_ERR  ERR_LOG(MongoDBTreeGroupDataAccess)

const boost::regex PathRegularExpression("(\\/[a-zA-Z0-9]+)+");

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBTreeGroupDataAccess::MongoDBTreeGroupDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
TreeGroupDataAccess(){}

MongoDBTreeGroupDataAccess::~MongoDBTreeGroupDataAccess() {}

#pragma mark Prtoected Method

//check the three path
bool MongoDBTreeGroupDataAccess::checkPathSintax(const std::string& tree_path) {
    bool result = false;
    boost::cmatch what;
    if(!(result = boost::regex_match(tree_path.c_str(), what, PathRegularExpression))) {
        //error recognizing the path
        MDBTGDA_ERR << "The tree path " << tree_path << " is not well formed";
    }
    return result;
}

int MongoDBTreeGroupDataAccess::checkPathPresenceForDomain(const std::string& group_domain,
                                                            const std::string& tree_path,
                                                            bool& presence) {
    int err = 0;
    unsigned long long count;
    std::vector< std::string > tree_path_node;
    try {
        //chec if the path is well formed
        if(!checkPathSintax(tree_path)) {
            return -10000;
        }
        //split the path in the node elements
        boost::split(tree_path_node,
                     tree_path, boost::is_any_of("/"),
                     boost::token_compress_on );
        
        //estract the node name that is the element in the front of the vector
        const std::string node_name = tree_path_node.front();
        
        //remove node name and slash from path for get parent path
        std::string tree_path_tmp = tree_path;
        tree_path_tmp.resize(tree_path_tmp.size()-(node_name.size()+1));
        
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("node_name" << node_name <<
                                "node_parent_path" << tree_path_tmp <<
                                "node_domain"<< group_domain);
        
        
        DEBUG_CODE(MDBTGDA_ERR<<log_message("checkPathPresenceForDomain",
                                            "count",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->count(count,
                                   MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_TREE_GROUP),
                                   q))){
            MDBTGDA_ERR << "Error checing the presence of node "<<node_name<<" of the path "<< tree_path_tmp << " with code " << err;
        }
    } catch (const mongo::DBException &e) {
        MDBTGDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBTGDA_ERR << e.what();
        err = e.errorCode;
    }
    
    return err;
}

#pragma mark Public Method
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