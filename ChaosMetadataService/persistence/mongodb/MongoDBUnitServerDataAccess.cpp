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

#define MDBUSDA_INFO INFO_LOG(MongoDBProducerDataAccess)
#define MDBUSDA_DBG  INFO_DBG(MongoDBProducerDataAccess)
#define MDBUSDA_ERR  INFO_ERR(MongoDBProducerDataAccess)

using namespace chaos::metadata_service::persistence::mongodb;

MongoDBUnitServerDataAccess::MongoDBUnitServerDataAccess() {
    
}

MongoDBUnitServerDataAccess::~MongoDBUnitServerDataAccess() {
    
}

//inherited method
int MongoDBUnitServerDataAccess::insertNewUnitServer(const std::string& unit_server_name,
                                                     const std::string& unit_server_rpc_url,
                                                     std::vector<std::string> hosted_cu_class) {
    int err = 0;
    bool f_exists = false;
    
    
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_insert_unit_server;
    
    try {
        //bson_insert_unit_server <<
        //bson_search.append(MONGO_DB_FIELD_FILE_VFS_PATH, vfs_file->getVFSFileInfo()->vfs_fpath);
        //bson_search.append(MONGO_DB_FIELD_FILE_VFS_DOMAIN, vfs_file->getVFSFileInfo()->vfs_domain);
       // mongo::BSONObj q = bson_search.obj();
       // DEBUG_CODE(MDBUSDA_DBG << "vfsAddNewDataBlock update ---------------------------------------------";)
       // DEBUG_CODE(MDBUSDA_DBG << "Query: "  << q.jsonString();)
       // DEBUG_CODE(MDBUSDA_DBG << "vfsAddNewDataBlock update ---------------------------------------------";)

       // ha_connection_pool->insert(MONGO_DB_COLLECTION_NAME(db_name, MONGO_DB_COLLECTION_VFS_VBLOCK),  i);

    } catch (const mongo::DBException &e) {
       // MDBID_LERR_ << e.what();
        err = -5;
    }
    return err;
}