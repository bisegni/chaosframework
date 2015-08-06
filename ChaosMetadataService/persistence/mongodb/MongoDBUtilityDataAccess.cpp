/*
 *	MongoDBUtilityDataAccess.cpp
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
#include "MongoDBUtilityDataAccess.h"

#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBUDA_INFO INFO_LOG(MongoDBUtilityDataAccess)
#define MDBUDA_DBG  DBG_LOG(MongoDBUtilityDataAccess)
#define MDBUDA_ERR  ERR_LOG(MongoDBUtilityDataAccess)

using namespace chaos::metadata_service::persistence::mongodb;
MongoDBUtilityDataAccess::MongoDBUtilityDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){

}

MongoDBUtilityDataAccess::~MongoDBUtilityDataAccess() {

}


int MongoDBUtilityDataAccess::getNextSequenceValue(const std::string& sequence_name, uint64_t& next_value) {
    int err = 0;
    mongo::BSONObj          result;
    mongo::BSONObjBuilder   query;
    mongo::BSONObjBuilder   update;
    try {
        query << "seq" << sequence_name;
        mongo::BSONObj q = query.obj();

            //try to sse if we need to initialize
        mongo::BSONObj ui = BSON("$setOnInsert" << BSON("value" << (int32_t)1));

        DEBUG_CODE(MDBUDA_DBG<<log_message("getNextSequenceValue",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update[upsert]",
                                                                   q.toString(),
                                                                   ui.toString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SEQUENCES), q, ui, true))) {
            MDBUDA_ERR << "Error initilizing ";
        } else {
            update << "$inc" << BSON("value" << 1);
            mongo::BSONObj u = update.obj();

            DEBUG_CODE(MDBUDA_DBG<<log_message("getNextSequenceValue",
                                               "findAndModify",
                                               DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                       "Update",
                                                                       q.toString(),
                                                                       u.toString()));)

            if((err = connection->findAndModify(result, MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SEQUENCES), q, u, false, false))) {
                MDBUDA_ERR << "Error updating ";
            } else if (!result.hasField("value")){
                MDBUDA_ERR << "Error retriving the sequence value";
                err = -1;
            } else {
                next_value = (uint64_t)result["value"].numberLong();
            }
        }
    } catch (const mongo::DBException &e) {
        MDBUDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBUtilityDataAccess::resetAllData() {
    int err = 0;
    try {
        mongo::BSONObj q;
        
        MDBUDA_DBG<<"Start resetting mds data";
        
        
        const char* const collection[] = {MONGODB_COLLECTION_SEQUENCES,
            MONGODB_COLLECTION_NODES,
            MONGODB_COLLECTION_NODES_COMMAND,
            MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE};
        
        for(int idx = 0;
            idx < 4;
            idx++) {
            MDBUDA_DBG<<"Remove all documents for:" << collection[idx];
            if((err = connection->remove(MONGO_DB_COLLECTION_NAME(collection[idx]), q, false))) {
                MDBUDA_ERR << "Error removing all documents from:" << collection[idx];
            }
        }
    } catch (const mongo::DBException &e) {
        MDBUDA_ERR << e.what();
        err = -1;
    }
    return err;
}