/*
 *	MongoDBUnitServerDataAccess.cpp
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
#include "MongoDBUnitServerDataAccess.h"
#include "mongo_db_constants.h"


#define MDBUSDA_INFO INFO_LOG(MongoDBUnitServerDataAccess)
#define MDBUSDA_DBG  DBG_LOG(MongoDBUnitServerDataAccess)
#define MDBUSDA_ERR  ERR_LOG(MongoDBUnitServerDataAccess)

using namespace chaos::common::data;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBUnitServerDataAccess::MongoDBUnitServerDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
    
}

MongoDBUnitServerDataAccess::~MongoDBUnitServerDataAccess() {
    
}

//inherited method
int MongoDBUnitServerDataAccess::insertNewUnitServer(CDataWrapper& unit_server_description) {
    int err = 0;
    try {
        std::auto_ptr<SerializationBuffer> ser(unit_server_description.getBSONData());
        mongo::BSONObj obj_to_insert(ser->getBufferPtr());
        
        DEBUG_CODE(MDBUSDA_DBG << "insertNewUnitServer insert ---------------------------------------------";)
        DEBUG_CODE(MDBUSDA_DBG << "Query: "  << obj_to_insert.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "insertNewUnitServer insert ---------------------------------------------";)
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_UNIT_SERVER),
                                    obj_to_insert))) {
            MDBUSDA_ERR << "Error inserting new unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBUSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

//inherited method
int MongoDBUnitServerDataAccess::checkUnitServerPresence(const std::string& unit_server_alias,
                                                         bool& presence) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObj result;
    try {
        bson_find << MONGODB_KEY_UNIT_SERVER_ALIAS << unit_server_alias;
        mongo::BSONObj q = bson_find.obj();
        DEBUG_CODE(MDBUSDA_DBG << "checkUnitServerPresence find ---------------------------------------------";)
        DEBUG_CODE(MDBUSDA_DBG << "Query: "  << q.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "checkUnitServerPresence find ---------------------------------------------";)
        if((err = connection->findOne(result,
                            MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_UNIT_SERVER),
                                     q))){
             MDBUSDA_ERR << "Error sercing unit server unit server" << unit_server_alias;
        }
    } catch (const mongo::DBException &e) {
        MDBUSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBUnitServerDataAccess::updateUnitServer(chaos::common::data::CDataWrapper& unit_server_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
        bson_find << MONGODB_KEY_UNIT_SERVER_ALIAS << unit_server_description.getStringValue(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_ALIAS);
        updated_field << MONGODB_KEY_UNIT_SERVER_RPC_ADDR << unit_server_description.getStringValue(CUDefinitionKey::CU_INSTANCE_NET_ADDRESS);
        
        mongo::BSONArrayBuilder bab;
        auto_ptr<CMultiTypeDataArrayWrapper> cu_type_array(unit_server_description.getVectorValue(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS));
        for(int idx = 0;
            idx < cu_type_array->size();
            idx++) {
            bab.append(cu_type_array->getStringElementAtIndex(idx));
        }
        
        updated_field.appendArray(MONGODB_KEY_UNIT_SERVER_CU_TYPE, bab.arr());

        mongo::BSONObj query = bson_find.obj();
        
        //set the update
        bson_update << "$set" << bson_update.obj();
        mongo::BSONObj update = bson_update.obj();
        DEBUG_CODE(MDBUSDA_DBG << "updateUnitServer find ---------------------------------------------";)
        DEBUG_CODE(MDBUSDA_DBG << "Query: "  << query.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "Update: "  << update.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "updateUnitServer find ---------------------------------------------";)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_UNIT_SERVER),
                                     query,
                                     update))) {
            MDBUSDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBUSDA_ERR << e.what();
        err = -1;
    }
    return err;
}
