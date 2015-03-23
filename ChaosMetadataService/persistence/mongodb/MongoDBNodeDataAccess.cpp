/*
 *	MongoDBNodeDataAccess.cpp
 *	!CHOAS
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
#include <mongo/client/dbclient.h>
#include "MongoDBNodeDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/utility/TimingUtil.h>
#define MDBNDA_INFO INFO_LOG(MongoDBNodeDataAccess)
#define MDBNDA_DBG  DBG_LOG(MongoDBNodeDataAccess)
#define MDBNDA_ERR  ERR_LOG(MongoDBNodeDataAccess)

using namespace chaos::common::data;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBNodeDataAccess::MongoDBNodeDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){}

MongoDBNodeDataAccess::~MongoDBNodeDataAccess() {}

int MongoDBNodeDataAccess::getNodeDescription(const std::string& node_unique_id,
                                              chaos::common::data::CDataWrapper **node_description) {
    int err = 0;
    mongo::BSONObj result;
    mongo::BSONObjBuilder query_builder;
    try {
        if(node_unique_id.size() == 0) return -1; //invalid unique id
        query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        
        mongo::BSONObj q = query_builder.obj();
        DEBUG_CODE(MDBNDA_DBG << "getNodeDescription findOne ---------------------------------------------";)
        DEBUG_CODE(MDBNDA_DBG << "Query: "  << q.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "getNodeDescription findOne ---------------------------------------------";)
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES), q))){
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
    }
    return err;
}

//inherited method
int MongoDBNodeDataAccess::insertNewNode(CDataWrapper& node_description) {
    int err = 0;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) return -2;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) return -3;
            //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) return -4;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) return -5;
        
        std::auto_ptr<SerializationBuffer> ser(node_description.getBSONData());
        mongo::BSONObj obj_to_insert(ser->getBufferPtr());
        
        DEBUG_CODE(MDBNDA_DBG << "insertNewNode insert ---------------------------------------------";)
        DEBUG_CODE(MDBNDA_DBG << "Query: "  << obj_to_insert.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "insertNewNode insert ---------------------------------------------";)
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                     obj_to_insert))) {
            MDBNDA_ERR << "Error inserting new unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! update the node updatable feature
int MongoDBNodeDataAccess::updateNode(chaos::common::data::CDataWrapper& node_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) return -2;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) return -4;
       
        const std::string& node_unique_id = node_description.getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
        const std::string& node_rpc_addr = node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
        const uint64_t node_timestamp = node_description.getUInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP);
        const std::string& node_rpc_domain = node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)?node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN):"";


        //serach criteria
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        //update field
        updated_field << chaos::NodeDefinitionKey::NODE_RPC_ADDR << node_rpc_addr
        << chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << node_rpc_domain
        << chaos::NodeDefinitionKey::NODE_TIMESTAMP << (long long)node_timestamp;

        mongo::BSONObj query = bson_find.obj();
        
        //set the update
        bson_update << "$set" << updated_field.obj();
        mongo::BSONObj update = bson_update.obj();
        DEBUG_CODE(MDBNDA_DBG << "updateUS update ---------------------------------------------";)
        DEBUG_CODE(MDBNDA_DBG << "Query: "  << query.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "Update: "  << update.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "updateUS update ---------------------------------------------";)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBNDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//inherited method
int MongoDBNodeDataAccess::checkNodePresence(const std::string& node_unique_id,
                                             bool& presence) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObj result;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        mongo::BSONObj q = bson_find.obj();
        DEBUG_CODE(MDBNDA_DBG << "checkNodePresence find ---------------------------------------------";)
        DEBUG_CODE(MDBNDA_DBG << "Query: "  << q.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "checkNodePresence find ---------------------------------------------";)
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                      q))){
            MDBNDA_ERR << "Error sercing unit server unit server" << node_unique_id;
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::deleteNode(const std::string& node_unique_id) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        mongo::BSONObj q = bson_find.obj();
        DEBUG_CODE(MDBNDA_DBG << "deleteNode delete ---------------------------------------------";)
        DEBUG_CODE(MDBNDA_DBG << "Query: "  << q.jsonString();)
        DEBUG_CODE(MDBNDA_DBG << "deleteNode delete ---------------------------------------------";)
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                    q))){
            MDBNDA_ERR << "Error deleting unit server" << node_unique_id;
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}