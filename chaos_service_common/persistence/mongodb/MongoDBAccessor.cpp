/*
 *	MongoDBAccessor.cpp
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

#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>
#include <chaos/common/global.h>
#define MDBACC_INFO INFO_LOG(MongoDBAccessor)
#define MDBACC_DBG  DBG_LOG(MongoDBAccessor)
#define MDBACC_ERR  ERR_LOG(MongoDBAccessor)

using namespace chaos::service_common::persistence::mongodb;

MongoDBAccessor::MongoDBAccessor(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
connection(_connection) {
    
}

MongoDBAccessor::~MongoDBAccessor() {
    
}

const std::string& MongoDBAccessor::getDatabaseName() {
    return connection->getDatabaseName();
}

mongo::BSONObj MongoDBAccessor::regexOnField(const std::string& field_name,
                                             const std::string& regex) {
    return MONGODB_REGEX_ON_FILED(field_name, regex);
}

mongo::BSONObj MongoDBAccessor::arrayMatch(const std::string& serach_key, const std::string& search_value) {
    return BSON("$elemMatch" << BSON(serach_key << search_value));
}

mongo::BSONObj MongoDBAccessor::arrayMatch(const std::vector<std::pair<std::string, std::string> >& search_keys_values) {
    mongo::BSONObjBuilder q_math_build;
        //scan all search info within array
    for(std::vector<std::pair<std::string, std::string> >::const_iterator it = search_keys_values.begin();
        it != search_keys_values.end();
        it++){
        q_math_build << it->first << it->second;
    }

    return BSON("$elemMatch" << q_math_build.obj());
}

int MongoDBAccessor::performPagedQuery(SearchResult& paged_result,
                                       const std::string& db_collection,
                                       mongo::Query q,
                                       mongo::BSONObj *prj,
                                       mongo::BSONObj *from,
                                       uint32_t limit) {
    int err = 0;
    try {
        //set the update
        DEBUG_CODE(MDBACC_DBG << "performPagedQuery ---------------------------------------------";)
        DEBUG_CODE(MDBACC_DBG << "Query: "  << q.toString();)
        if(prj) DEBUG_CODE(MDBACC_DBG << "Projection: "  << prj->toString();)
        if(from) DEBUG_CODE(MDBACC_DBG << "Starting from: "  << from->toString();)
        DEBUG_CODE(MDBACC_DBG << "Limit: "  << limit;)
        DEBUG_CODE(MDBACC_DBG << "performPagedQuery ---------------------------------------------";)
        connection->findN(paged_result, db_collection, q, limit, 0, prj);
    } catch (const mongo::DBException &e) {
        MDBACC_ERR << e.what();
        err = e.getCode();
    } catch (const chaos::CException &e) {
        MDBACC_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}