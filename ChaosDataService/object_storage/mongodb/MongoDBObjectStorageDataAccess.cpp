/*
 *	MongoDBObjectStorageDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#include "MongoDBObjectStorageDataAccess.h"

using namespace chaos::data_service::object_storage::mongodb;

#include "MongoDBObjectStorageDataAccess.h"

#include <chaos/common/utility/TimingUtil.h>

#define INFO INFO_LOG(MongoDBObjectStorageDataAccess)
#define DBG  DBG_LOG(MongoDBObjectStorageDataAccess)
#define ERR  ERR_LOG(MongoDBObjectStorageDataAccess)

using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::data_service::object_storage::mongodb;

MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string& key,
                                               const uint64_t& timestamp,
                                               const chaos::common::data::CDataWrapper& stored_object) {
    int err = 0;
    /* mongo::BSONObj result;
     mongo::BSONObjBuilder query_builder;
     try {
     if(node_unique_id.size() == 0) return -1; //invalid unique id
     query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
     
     mongo::BSONObj q = query_builder.obj();
     mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
     chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
     chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_TIMESTAMP << 1);
     
     DEBUG_CODE(MDBNDA_DBG<<log_message("getNodeDescription",
     "findOne",
     DATA_ACCESS_LOG_2_ENTRY("Query",
     "Projection",
     q.jsonString(),
     p.jsonString()));)
     
     if((err = connection->findOne(result,
     MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, &p))){
     MDBNDA_ERR << "Error fetching node description";
     } else if(result.isEmpty()) {
     MDBNDA_ERR << "No node description has been found";
     err = -2;
     } else {
     *node_description = new CDataWrapper(result.objdata());
     }
     } catch (const mongo::DBException &e) {
     MDBNDA_ERR << e.what();
     err = e.getCode();
     }*/
    return err;
}

int MongoDBObjectStorageDataAccess::getObject(const std::string& key,
                                              const uint64_t& timestamp,
                                              object_storage::abstraction::ObjectSharedPtr& object_ptr_ref) {
    int err = 0;
    /* mongo::BSONObj result;
     mongo::BSONObjBuilder query_builder;
     try {
     if(node_unique_id.size() == 0) return -1; //invalid unique id
     query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
     
     mongo::BSONObj q = query_builder.obj();
     mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
     chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
     chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_TIMESTAMP << 1);
     
     DEBUG_CODE(MDBNDA_DBG<<log_message("getNodeDescription",
     "findOne",
     DATA_ACCESS_LOG_2_ENTRY("Query",
     "Projection",
     q.jsonString(),
     p.jsonString()));)
     
     if((err = connection->findOne(result,
     MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, &p))){
     MDBNDA_ERR << "Error fetching node description";
     } else if(result.isEmpty()) {
     MDBNDA_ERR << "No node description has been found";
     err = -2;
     } else {
     *node_description = new CDataWrapper(result.objdata());
     }
     } catch (const mongo::DBException &e) {
     MDBNDA_ERR << e.what();
     err = e.getCode();
     }*/
    return err;
}
int MongoDBObjectStorageDataAccess::findObject(const std::string& key,
                                               const uint64_t& timestamp_from,
                                               const uint64_t& timestamp_to,
                                               const uint64_t& last_got_ts,
                                               object_storage::abstraction::VectorObject& found_object_page) {
    int err = 0;
    /* mongo::BSONObj result;
     mongo::BSONObjBuilder query_builder;
     try {
     if(node_unique_id.size() == 0) return -1; //invalid unique id
     query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
     
     mongo::BSONObj q = query_builder.obj();
     mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
     chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
     chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
     chaos::NodeDefinitionKey::NODE_TIMESTAMP << 1);
     
     DEBUG_CODE(MDBNDA_DBG<<log_message("getNodeDescription",
     "findOne",
     DATA_ACCESS_LOG_2_ENTRY("Query",
     "Projection",
     q.jsonString(),
     p.jsonString()));)
     
     if((err = connection->findOne(result,
     MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, &p))){
     MDBNDA_ERR << "Error fetching node description";
     } else if(result.isEmpty()) {
     MDBNDA_ERR << "No node description has been found";
     err = -2;
     } else {
     *node_description = new CDataWrapper(result.objdata());
     }
     } catch (const mongo::DBException &e) {
     MDBNDA_ERR << e.what();
     err = e.getCode();
     }*/
    return err;
}
