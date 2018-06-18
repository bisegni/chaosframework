/*
 * Copyright 2012, 18/06/2018 INFN
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
#include <chaos_service_common/persistence/mongodb/mongodb_cxx/MongoDBCXXDriver.h>

#include <chaos/common/global.h>

using namespace chaos::service_common::persistence::mongodb::mongodb_cxx;

mongocxx::instance BaseMongoDBDiver::drv_instance = {};

BaseMongoDBDiver::BaseMongoDBDiver() {
    
}

BaseMongoDBDiver::~BaseMongoDBDiver() {
    
}

void BaseMongoDBDiver::initPool(const ChaosStringVector& url_list) {
    std::string host_composed_url = "localhost:27017";
    if(url_list.size()) {
        for(ChaosStringVectorConstIterator it = url_list.begin(),
            end = url_list.end();
            it != end;
            it++) {
            host_composed_url.append(*it);
            host_composed_url.append(",");
        }
        host_composed_url.resize(host_composed_url.size()-1);
    }
    mongocxx::uri uri{CHAOS_FORMAT("mongodb://%1%/?minPoolSize=%2%&maxPoolSize=%3%",%""%100%100)};
    pool_unique_ptr.reset(new mongocxx::pool());
}

mongocxx::pool& BaseMongoDBDiver::getPool() {
    CHAOS_ASSERT(pool_unique_ptr)
    return *pool_unique_ptr;
}
