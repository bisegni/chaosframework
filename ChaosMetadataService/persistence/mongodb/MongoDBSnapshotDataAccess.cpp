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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/NetworkBroker.h>

#include <mongo/client/dbclient.h>

#define MDBDSDA_INFO INFO_LOG(MongoDBSnapshotDataAccess)
#define MDBDSDA_DBG  DBG_LOG(MongoDBSnapshotDataAccess)
#define MDBDSDA_ERR  ERR_LOG(MongoDBSnapshotDataAccess)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

MongoDBSnapshotDataAccess::MongoDBSnapshotDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                                     data_access::DataServiceDataAccess *_data_service_da):
MongoDBAccessor(_connection),
SnapshotDataAccess(_data_service_da){}

MongoDBSnapshotDataAccess::~MongoDBSnapshotDataAccess() {}

int MongoDBSnapshotDataAccess::createNewSnapshot(const std::string& snapshot_name,
                                                 const ChaosStringVector& node_uid_list) {
    return SnapshotDataAccess::createNewSnapshot(snapshot_name,
                                                 node_uid_list);
}

int MongoDBSnapshotDataAccess::getNodeInSnapshot(const std::string& snapshot_name,
                                                 ChaosStringVector& node_in_snapshot) {
    int err = 0;
    bool work_free = false;
    std::vector<mongo::BSONObj>     result;
    try {
        
        if((err = getSnapshotWorkingState(snapshot_name, work_free))){
            return err;
        } if(work_free == false) {
            MDBDSDA_ERR << "Snapshot " << snapshot_name << " is still be elaborated";
            return -10000;
        }
        
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

int MongoDBSnapshotDataAccess::deleteSnapshot(const std::string& snapshot_name) {
    int err = 0;
    bool work_free = false;
    if((err = getSnapshotWorkingState(snapshot_name, work_free))){
        return err;
    } if(work_free == false) {
        MDBDSDA_ERR << "Snapshot " << snapshot_name << " is still be elaborated";
        return -10000;
    }
    return SnapshotDataAccess::deleteSnapshot(snapshot_name);
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
