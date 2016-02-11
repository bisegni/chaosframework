/*
 *	MongoDBLoggingDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 11/02/16 INFN, National Institute of Nuclear Physics
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
#include "MongoDBLoggingDataAccess.h"

#include <mongo/client/dbclient.h>

#define MDBLDA_INFO INFO_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_DBG  DBG_LOG(MongoDBLoggingDataAccess)
#define MDBLDA_ERR  ERR_LOG(MongoDBLoggingDataAccess)

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBLoggingDataAccess::MongoDBLoggingDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
LoggingDataAccess(){}

MongoDBLoggingDataAccess::~MongoDBLoggingDataAccess() {}

int MongoDBLoggingDataAccess::insertNewEntry(const data_access::LogEntry& log_entry) {
    return 0;
}