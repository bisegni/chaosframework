/*
 *	MongoDBControlUnitDataAccess.cpp
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
#include "mongo_db_constants.h"
#include "MongoDBControlUnitDataAccess.h"

#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBCUDA_INFO INFO_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_DBG  DBG_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_ERR  ERR_LOG(MongoDBControlUnitDataAccess)

using namespace chaos::common::data;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBControlUnitDataAccess::MongoDBControlUnitDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
    
}

MongoDBControlUnitDataAccess::~MongoDBControlUnitDataAccess() {
    
}

int MongoDBControlUnitDataAccess::insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description) {
    int err = 0;
    return err;
}

int MongoDBControlUnitDataAccess::updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description) {
    int err = 0;
    return err;
}

int MongoDBControlUnitDataAccess::addNewDataset(chaos::common::data::CDataWrapper& dataset_description) {
    int err = 0;
    return err;
}


int MongoDBControlUnitDataAccess::checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description)  {
    int err = 0;
    return err;
}


int MongoDBControlUnitDataAccess::getLastDataset(chaos::common::data::CDataWrapper& dataset_description)  {
    int err = 0;
    return err;
}
