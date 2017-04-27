/*
 *	MongoDBSnapshotDataAccess.h
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
#include "MongoDBSnapshotDataAccess.h"

#include <chaos/common/data/CDataWrapper.h>

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/NetworkBroker.h>

#include <mongo/client/dbclient.h>

#define MDBDSDA_INFO INFO_LOG(MongoDBSnapshotDataAccess)
#define MDBDSDA_DBG  DBG_LOG(MongoDBSnapshotDataAccess)
#define MDBDSDA_ERR  ERR_LOG(MongoDBSnapshotDataAccess)

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

MongoDBSnapshotDataAccess::MongoDBSnapshotDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                                     data_access::DataServiceDataAccess *_data_service_da):
MongoDBAccessor(_connection),
SnapshotDataAccess(_data_service_da){}

MongoDBSnapshotDataAccess::~MongoDBSnapshotDataAccess() {}

//! Create a new snapshot
int MongoDBSnapshotDataAccess::snapshotCreateNewWithName(const std::string& snapshot_name,
                                                         std::string& working_job_unique_id) {
    int err = 0;
    mongo::BSONObjBuilder	new_snapshot_start;
    mongo::BSONObj          check_unique_q;
    mongo::BSONObj          check_result;
    //----- generate the random code ------ for this snapshot
    working_job_unique_id = UUIDUtil::generateUUIDLite();
    try{
        
        check_unique_q = BSON(MONGO_DB_FIELD_SNAPSHOT_NAME << snapshot_name);
        
        uint64_t ts = TimingUtil::getTimeStamp();
        new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_NAME << snapshot_name;
        new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_TS_DATE << mongo::Date_t(ts);
        new_snapshot_start << MONGO_DB_FIELD_SNAPSHOT_TS << (long long)ts;
        new_snapshot_start << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
        mongo::BSONObj q = new_snapshot_start.obj();
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotCreateNewWithName",
                                            "insert",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->findOne(check_result,
                                      MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT),
                                      check_unique_q))) {//check existence
            MDBDSDA_ERR << CHAOS_FORMAT("Error %1% checking existence snapshot %2%", %err%snapshot_name);
        } else if(check_result.isEmpty() == false) {//if false means that a document has been found
            MDBDSDA_ERR << CHAOS_FORMAT("A snapshot for name %1% already exists", %snapshot_name);
            err = -1;
        } else if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT), q))) {//we can proceeed to insert the new snapshot
            MDBDSDA_ERR << CHAOS_FORMAT("Error %1% creating snapshot %2%", %err%snapshot_name);
        }
    } catch( const mongo::DBException &e ) {
        MDBDSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBSnapshotDataAccess::isSnapshotPresent(const std::string& snapshot_name,
                                                 bool& presence) {
    int err = 0;
    try {
        mongo::BSONObj result;
        //search for rigth node snpashot slot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name);
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getDatasetInSnapshotForNode",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT),
                                      q))) {
            MDBDSDA_ERR << CHAOS_FORMAT("Error %1% searching the snapshot %2%", %err%snapshot_name);
        }
        presence = (result.isEmpty() == false);
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! Add an element to a named snapshot
int MongoDBSnapshotDataAccess::snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
                                                            const std::string& snapshot_name,
                                                            const std::string& producer_unique_key,
                                                            const std::string& dataset_type,
                                                            void* data,
                                                            uint32_t data_len) {
    int err = 0;
    mongo::BSONObjBuilder	new_dataset;
    mongo::BSONObjBuilder	search_snapshot;
    try{
        new_dataset << "$set"<< BSON(dataset_type << mongo::BSONObj((const char *)data));
        
        //search for snapshot name and producer unique key
        search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
        search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID << producer_unique_key;
        search_snapshot << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
        
        mongo::BSONObj u = new_dataset.obj();
        mongo::BSONObj q = search_snapshot.obj();
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotAddElementToSnapshot",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.jsonString(),
                                                                    u.toString()));)
        
        //update and waith until the data is on the server
        err = connection->update(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT_DATA), q, u, true, false);
    } catch( const mongo::DBException &e ) {
        MDBDSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBSnapshotDataAccess::snapshotIncrementJobCounter(const std::string& working_job_unique_id,
                                                           const std::string& snapshot_name,
                                                           bool add) {
    int err = 0;
    mongo::BSONObjBuilder	inc_update;
    mongo::BSONObjBuilder	search_snapshot;
    try{
        inc_update << "$inc"<< BSON(MONGO_DB_FIELD_SNAPSHOT_JOB_CONCURRENCY<< (add?1:-1));
        
        //search for snapshot name and producer unique key
        search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
        search_snapshot << MONGO_DB_FIELD_JOB_WORK_UNIQUE_CODE << working_job_unique_id;
        
        mongo::BSONObj u = inc_update.obj();
        mongo::BSONObj q = search_snapshot.obj();
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotIncrementJobCounter",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.jsonString(),
                                                                    u.toString()));)
        
        
        //update and waith until the data is on the server
        err = connection->update(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT), q, u, false, false);
    } catch( const mongo::DBException &e ) {
        MDBDSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

//! get the dataset from a snapshot
int MongoDBSnapshotDataAccess::snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
                                                                const std::string& producer_unique_key,
                                                                const std::string& dataset_type,
                                                                void **channel_data,
                                                                uint32_t& channel_data_size) {
    int err = 0;
    mongo::BSONObj			result;
    mongo::BSONObjBuilder	search_snapshot;
    try{
        //search for snapshot name and producer unique key
        search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name
        << MONGO_DB_FIELD_SNAPSHOT_DATA_PRODUCER_ID << producer_unique_key;
        
        mongo::BSONObj q = search_snapshot.obj();
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotGetDatasetForProducerKey",
                                            "finedOne",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        //update and waith until the data is on the server
        if((err = connection->findOne(result, MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))) {
            MDBDSDA_ERR << "Errore finding the snapshot "<< snapshot_name << " for the unique key "<< producer_unique_key <<"with error "<<err;
        } else {
            std::string channel_unique_key = producer_unique_key+dataset_type;
            if(result.hasField(channel_unique_key)) {
                //! get data
                mongo::BSONObj channel_data_obj = result.getObjectField(channel_unique_key);
                if((channel_data_size = channel_data_obj.objsize())) {
                    *channel_data = malloc(channel_data_size);
                    if(*channel_data) {
                        std::memcpy(*channel_data, (void*)channel_data_obj.objdata(), channel_data_size);
                    }
                }
            }
        }
    } catch( const mongo::DBException &e ) {
        MDBDSDA_ERR << e.what();
        err = -1;
    }
    return 0;
}

//! Delete a snapshot where no job is working
int MongoDBSnapshotDataAccess::snapshotDeleteWithName(const std::string& snapshot_name) {
    int err = 0;
    mongo::BSONObj			result;
    mongo::BSONObjBuilder	search_snapshot;
    try{
        //search for snapshot name and producer unique key
        search_snapshot << MONGO_DB_FIELD_SNAPSHOT_DATA_SNAPSHOT_NAME << snapshot_name;
        
        mongo::BSONObj q = search_snapshot.obj();
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotDeleteWithName",
                                            "findOne",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        //update and waith until the data is on the server
        if((err = connection->findOne(result, MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT), q))) {
            MDBDSDA_ERR << "Errore finding the snapshot "<< snapshot_name << "with error "<<err;
            return err;
        }
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotDeleteWithName",
                                            "remove Snapshot",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        //the snapshot to delete is present, so we delete it
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT), q))){
            MDBDSDA_ERR << "Errore deleting the snapshot "<< snapshot_name << "with error "<<err;
            return err;
        }
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("snapshotDeleteWithName",
                                            "remove Snapshot Data",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        //no we need to delete all dataset associated to it
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGO_DB_COLLECTION_SNAPSHOT_DATA), q))){
            MDBDSDA_ERR << "Errore deleting the snapshot data "<< snapshot_name << "with error "<<err;
        }
    } catch( const mongo::DBException &e ) {
        MDBDSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

int MongoDBSnapshotDataAccess::getNodeInSnapshot(const std::string& snapshot_name,
                                                 ChaosStringVector& node_in_snapshot) {
    int err = 0;
    bool work_free = false;
    std::vector<mongo::BSONObj>     result;
    try {
        //        if((err = getSnapshotWorkingState(snapshot_name, work_free))){
        //            return err;
        //        } if(work_free == false) {
        //            MDBDSDA_ERR << "Snapshot " << snapshot_name << " is still be elaborated";
        //            return -10000;
        //        }
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name);
        
        //! project only the node id
        mongo::BSONObj prj = BSON("producer_id" << 1 << "_id"<<0);
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getNodeInSnapshot",
                                            "find",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "prj",
                                                                    q.jsonString(),
                                                                    prj.jsonString()));)
        
        std::auto_ptr<mongo::DBClientCursor> query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT_DATA),
                                                                              q,
                                                                              0,
                                                                              0,
                                                                              &prj);
        while(query_result->more()) {
            mongo::BSONObj n = query_result->next();
            node_in_snapshot.push_back(n.getStringField("producer_id"));
        }
        
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBSnapshotDataAccess::getSnapshotForNode(const std::string& node_unique_id,
                                                  ChaosStringVector& snapshot_for_node) {
    int err = 0;
    std::vector<mongo::BSONObj> result;
    try {
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("producer_id" << node_unique_id);
        
        //! project only the node id
        mongo::BSONObj prj = BSON("snap_name" << 1 << "_id"<<0);
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getSnapshotForNode",
                                            "find",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "prj",
                                                                    q.jsonString(),
                                                                    prj.jsonString()));)
        
        std::auto_ptr<mongo::DBClientCursor> query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT_DATA),
                                                                              q,
                                                                              0,
                                                                              0,
                                                                              &prj);
        while(query_result->more()) {
            mongo::BSONObj n = query_result->next();
            snapshot_for_node.push_back(n.getStringField("snap_name"));
        }
        
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBSnapshotDataAccess::getSnapshotWorkingState(const std::string& snapshot_name,
                                                       bool& work_free) {
    int err = 0;
    mongo::BSONObj  snapshot_info;
    try {
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name);
        
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getSnapshotWorkingState",
                                            "find",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        
        if((err = connection->findOne(snapshot_info,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT),
                                      q))) {
            MDBDSDA_ERR << "Error getting snapsho description with code:" << err;
        } else if(snapshot_info.isEmpty()) {
            err = - 10000;
            MDBDSDA_ERR << "Snapshto description has not been found" << err;
        } else {
            //we have snapshot description
            if(snapshot_info.hasField("job_concurency")) {
                work_free  = snapshot_info.getIntField("job_concurency") == 0;
            } else {
                work_free = false;
            }
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    
    return err;
}

int MongoDBSnapshotDataAccess::getAllSnapshot(SnapshotList& snapshot_desriptions) {
    int err = 0;
    try {
        
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q;
        
        //! project only the node id
        mongo::BSONObj prj = BSON("snap_name" << 1 << "snap_ts" << 1 << "job_concurency" << 1);
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getAllSnapshot",
                                            "query",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "prj",
                                                                    q.jsonString(),
                                                                    prj.jsonString()));)
        
        std::auto_ptr<mongo::DBClientCursor> query_result = connection->query(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT),
                                                                              q,
                                                                              0,
                                                                              0,
                                                                              &prj);
        if(query_result.get()) {
            while(query_result->more()) {
                mongo::BSONObj n = query_result->next();
                snapshot_desriptions.push_back(SnapshotElementPtr(new CDataWrapper(n.objdata())));
            }
        }else{
            err = -10000;
        }
        
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBSnapshotDataAccess::getDatasetInSnapshotForNode(const std::string& node_unique_id,
                                                           const std::string& snapshot_name,
                                                           VectorStrCDWShrdPtr& snapshot_for_node) {
    int err = 0;
    try {
        mongo::BSONObj result;
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name << "producer_id" << node_unique_id);
        
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getDatasetInSnapshotForNode",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT_DATA),
                                      q)) == 0) {
            if(result.isEmpty() == false) {
                mongo::BSONObjIterator it = result.begin();
                while(it.more()) {
                    mongo::BSONElement element = it.next();
                    if(element.type() != mongo::Object) continue;
                    
                    //we get only object element that are the dataset of the snapshot
                    PairStrCDWShrdPtr ds_element(element.fieldNameStringData().toString(), CDWShrdPtr(new CDataWrapper(element.Obj().objdata())));
                    snapshot_for_node.push_back(ds_element);
                }
            }
        }
        
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBSnapshotDataAccess::setDatasetInSnapshotForNode(const std::string& working_job_unique_id,
                                                           const std::string& node_unique_id,
                                                           const std::string& snapshot_name,
                                                           const std::string& dataset_key,
                                                           common::data::CDataWrapper& dataset_value) {
    int err = 0;
    try {
        int size = 0;
        mongo::BSONObj result;
        //search for rigth node snpashot slot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name << "producer_id" << node_unique_id);
        mongo::BSONObj u = BSON(dataset_key << mongo::BSONObj(dataset_value.getBSONRawData(size)));
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getDatasetInSnapshotForNode",
                                            "query",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.jsonString(),
                                                                    u.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_SNAPSHOT_DATA),
                                     q,
                                     u,
                                     true,
                                     false,
                                     mongo::WriteConcern::acknowledged))) {
            MDBDSDA_ERR << CHAOS_FORMAT("Error %1% updateting dataset for node %2% in snapshot %3%", %err%node_unique_id%snapshot_name);
        }
        
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}
