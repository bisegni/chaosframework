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

MongoDBNodeDataAccess::MongoDBNodeDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
    
}

MongoDBNodeDataAccess::~MongoDBNodeDataAccess() {
    
}

//inherited method
int MongoDBNodeDataAccess::insertNewNode(CDataWrapper& unit_server_description) {
    int err = 0;
    try {
        std::auto_ptr<SerializationBuffer> ser(unit_server_description.getBSONData());
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
        err = -1;
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
        MDBNDA_ERR << e.what();
        err = -1;
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
        err = -1;
    }
    return err;
}