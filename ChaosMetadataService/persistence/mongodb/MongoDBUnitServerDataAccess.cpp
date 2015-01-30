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

#include "MongoDBUnitServerDataAccess.h"
#include "mongo_db_constants.h"

#define MDBUSDA_INFO INFO_LOG(MongoDBUnitServerDataAccess)
#define MDBUSDA_DBG  INFO_DBG(MongoDBUnitServerDataAccess)
#define MDBUSDA_ERR  INFO_ERR(MongoDBUnitServerDataAccess)

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
    
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_insert;
    
    try {
        std::auto_ptr<SerializationBuffer> ser(unit_server_description.getBSONData());
        mongo::BSONObj obj_to_insert(ser->getBufferPtr());

      // DEBUG_CODE(MDBUSDA_DBG << "insertNewUnitServer update ---------------------------------------------";)
      // DEBUG_CODE(MDBUSDA_DBG << "Query: "  << obj_to_insert.jsonString();)
//DEBUG_CODE(MDBUSDA_DBG << "insertNewUnitServer update ---------------------------------------------";)

       connection->insert(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_UNIT_SERVER),  obj_to_insert);

    } catch (const mongo::DBException &e) {
       // MDBUSDA_ERR << e.what();
        err = -1;
    }
    return err;
}

//inherited method
int MongoDBUnitServerDataAccess::checkUnitServerPresence(const std::string& unit_server_alias,
                                                         bool& presence) {
    return 0;
}