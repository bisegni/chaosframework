/*
 *	MongoDBDeployDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/03/2017 INFN, National Institute of Nuclear Physics
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

#include "MongoDBDeployDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::data::agent;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO    INFO_LOG(MongoDBScriptDataAccess)
#define ERR     DBG_LOG(MongoDBScriptDataAccess)
#define DBG     ERR_LOG(MongoDBScriptDataAccess)

MongoDBDeployDataAccess::MongoDBDeployDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
DeployDataAccess(),
utility_data_access(NULL),
node_data_access(NULL){}

MongoDBDeployDataAccess::~MongoDBDeployDataAccess() {}
