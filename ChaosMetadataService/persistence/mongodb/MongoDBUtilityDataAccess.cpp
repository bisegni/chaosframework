/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include "mongo_db_constants.h"
#include "MongoDBUtilityDataAccess.h"

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBUDA_INFO INFO_LOG(MongoDBUtilityDataAccess)
#define MDBUDA_DBG  DBG_LOG(MongoDBUtilityDataAccess)
#define MDBUDA_ERR  ERR_LOG(MongoDBUtilityDataAccess)

using namespace chaos::common::data;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBUtilityDataAccess::MongoDBUtilityDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){}

MongoDBUtilityDataAccess::~MongoDBUtilityDataAccess() {}

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
        mongo::BSONObj q = BSON("ndk_uid" << BSON("$ne" << NodeType::NODE_TYPE_DATA_SERVICE));
        
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

int MongoDBUtilityDataAccess::setVariable(const std::string& variable_name,
                                          chaos::common::data::CDataWrapper& cdw){
    int err = 0;
    int variable_size = 0;
    try {
        mongo::BSONObj q = BSON("variable_name" << variable_name);
        mongo::BSONObj u = BSON("$set" << BSON("variable_value" << mongo::BSONObj(cdw.getBSONRawData(variable_size))));
        DEBUG_CODE(MDBUDA_DBG<<log_message("setVariable",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   q.toString(),
                                                                   u.toString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_VARIABLES),
                                     q,
                                     u,
                                     true))) {
            MDBUDA_ERR << CHAOS_FORMAT("Error setting the variable %1% with error %2%", %variable_name%err);
        }
    }catch (const mongo::DBException &e) {
        MDBUDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBUtilityDataAccess::getVariable(const std::string& variable_name,
                                          chaos::common::data::CDataWrapper **cdw){
    int err = 0;
    try {
        mongo::BSONObj result;
        mongo::BSONObj q = BSON("variable_name" << variable_name);
        
        DEBUG_CODE(MDBUDA_DBG<<log_message("getVariable",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.toString()));)
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_VARIABLES),
                                      q))) {
            MDBUDA_ERR << CHAOS_FORMAT("Error find the variable '%1%' with error %2%", %variable_name%err);
        } else if((result.isEmpty() == false)&&
                  result.hasField("variable_value")){
            mongo::BSONElement e = result["variable_value"];
            if(e.type() == mongo::Object) {
                int buffer_len = 0;
                const char * bin_data = (const char * ) e.Obj().objdata();
                if(bin_data) {
                    *cdw = new CDataWrapper(bin_data);
                }
            }
        }
    }catch (const mongo::DBException &e) {
        MDBUDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBUtilityDataAccess::deleteVariable(const std::string& variable_name) {
    int err = 0;
    try {
        mongo::BSONObj q = BSON("variable_name" << variable_name);
        
        DEBUG_CODE(MDBUDA_DBG<<log_message("deleteVariable",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.toString()));)
        
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_VARIABLES),
                                     q))) {
            MDBUDA_ERR << CHAOS_FORMAT("Error deleting the variable %1% with error %2%", %variable_name%err);
        }
    }catch (const mongo::DBException &e) {
        MDBUDA_ERR << e.what();
        err = -1;
    }
    return err;
}
