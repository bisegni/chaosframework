/*
 *	MongoDBAgentDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#include "MongoDBAgentDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/bson/util/base64.h>
#include <chaos/common/utility/TimingUtil.h>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <json/json.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::data::dataset;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO    INFO_LOG(MongoDBScriptDataAccess)
#define ERR     DBG_LOG(MongoDBScriptDataAccess)
#define DBG     ERR_LOG(MongoDBScriptDataAccess)

MongoDBAgentDataAccess::MongoDBAgentDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
AgentDataAccess(),
utility_data_access(NULL){}

MongoDBAgentDataAccess::~MongoDBAgentDataAccess() {}


int MongoDBAgentDataAccess::insertUpdateAgentDescription(chaos::common::data::CDataWrapper& agent_description) {
    return 0;
}
