/*
 *	MongoDBAccessor.cpp
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
#include "MongoDBAccessor.h"
#include <chaos/common/global.h>
#define MDBACC_INFO INFO_LOG(MongoDBAccessor)
#define MDBACC_DBG  DBG_LOG(MongoDBAccessor)
#define MDBACC_ERR  ERR_LOG(MongoDBAccessor)

using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::service_common::persistence::mongodb;

MongoDBAccessor::MongoDBAccessor(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
connection(_connection) {
    
}

MongoDBAccessor::~MongoDBAccessor() {
    
}

const std::string& MongoDBAccessor::getDatabaseName() {
    return connection->getDatabaseName();
}

int MongoDBAccessor::performPagedQuery(std::vector<mongo::BSONObj>& paged_result,
                                       mongo::BSONObj q,
                                       mongo::BSONObj *prj,
                                       mongo::BSONObj *from,
                                       uint32_t limit) {
    int err = 0;
    try {
        
        //set the update
    
            
        DEBUG_CODE(MDBACC_DBG << "performPagedQuery ---------------------------------------------";)
        DEBUG_CODE(MDBACC_DBG << "Query: "  << q.jsonString();)
        if(prj) DEBUG_CODE(MDBACC_DBG << "Projection: "  << prj->jsonString();)
        if(from) DEBUG_CODE(MDBACC_DBG << "Starting from: "  << from->jsonString();)
        DEBUG_CODE(MDBACC_DBG << "Limit: "  << limit;)
        DEBUG_CODE(MDBACC_DBG << "performPagedQuery ---------------------------------------------";)
        
        
    } catch (const mongo::DBException &e) {
        MDBACC_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBACC_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}