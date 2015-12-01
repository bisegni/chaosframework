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

#include "MongoDBSnapshotDataAccess.h"

#include "mongo_db_constants.h"

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

MongoDBSnapshotDataAccess::MongoDBSnapshotDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                                     data_access::DataServiceDataAccess *_data_service_da):
MongoDBAccessor(_connection),
SnapshotDataAccess(_data_service_da){}

MongoDBSnapshotDataAccess::~MongoDBSnapshotDataAccess() {}

// inherited method
int MongoDBSnapshotDataAccess::createNewSnapshot(const std::string& snapshot_name,
                                                 const std::vector<std::string> node_uid_list) {
    return SnapshotDataAccess::createNewSnapshot(snapshot_name,
                                                 node_uid_list);
}

// inherited method
int MongoDBSnapshotDataAccess::getNodeInSnapshot(const std::string& snapshot_name,
                                                 vector<std::string> node_in_snapshot) {
    int err = 0;
    std::vector<mongo::BSONObj>     result;
    try {
        //we first need to fetch all node uid attacched to the snapshot
        mongo::BSONObj q = BSON("snap_name" << snapshot_name);
        
        //! project only the node id
        mongo::BSONObj prj = BSON("producer_id" << 1);
        
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

// inherited method
int MongoDBSnapshotDataAccess::deleteSnapshot(const std::string& snapshot_name) {
    return SnapshotDataAccess::deleteSnapshot(snapshot_name);
}