/*
 * Copyright 2012, 2017 INFN
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

MongoDBDeployDataAccess::MongoDBDeployDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
DeployDataAccess(),
utility_data_access(NULL),
node_data_access(NULL){}

MongoDBDeployDataAccess::~MongoDBDeployDataAccess() {}
