/*
 *	MongoDBPersistenceDriver.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "MongoDBPersistenceDriver.h"
#include "MongoDBNodeDataAccess.h"
#include "MongoDBUtilityDataAccess.h"
#include "MongoDBUnitServerDataAccess.h"
#include "MongoDBControlUnitDataAccess.h"
#include "MongoDBDataServiceDataAccess.h"
#include "MongoDBSnapshotDataAccess.h"
#include "MongoDBTreeGroupDataAccess.h"
#include "MongoDBLoggingDataAccess.h"
#include "MongoDBScriptDataAccess.h"

#include "../../mds_types.h"

using namespace chaos;
using namespace chaos::metadata_service::persistence;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::service_common::persistence::data_access;
using namespace chaos::service_common::persistence::mongodb;

DEFINE_CLASS_FACTORY(MongoDBPersistenceDriver, chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

MongoDBPersistenceDriver::MongoDBPersistenceDriver(const std::string& name):
AbstractPersistenceDriver(name){
	
}
MongoDBPersistenceDriver::~MongoDBPersistenceDriver() {
	
}

void MongoDBPersistenceDriver::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::init(init_data);
    
	struct setting *_setting = static_cast<struct setting *>(init_data);
	if(!_setting) throw CException(-1, "No setting forwarded", __PRETTY_FUNCTION__);
	
	//we can configura the connection
	connection.reset(new MongoDBHAConnectionManager(_setting->persistence_server_list,
													_setting->persistence_kv_param_map));
    
    //register the data access implementations
    registerDataAccess<data_access::UnitServerDataAccess>(new MongoDBUnitServerDataAccess(connection));
    registerDataAccess<data_access::NodeDataAccess>(new MongoDBNodeDataAccess(connection));
    registerDataAccess<data_access::ControlUnitDataAccess>(new MongoDBControlUnitDataAccess(connection));
    registerDataAccess<data_access::UtilityDataAccess>(new MongoDBUtilityDataAccess(connection));
    registerDataAccess<data_access::DataServiceDataAccess>(new MongoDBDataServiceDataAccess(connection));
    registerDataAccess<data_access::SnapshotDataAccess>(new MongoDBSnapshotDataAccess(connection, getDataAccess<data_access::DataServiceDataAccess>()));
    registerDataAccess<data_access::TreeGroupDataAccess>(new MongoDBTreeGroupDataAccess(connection));
    registerDataAccess<data_access::LoggingDataAccess>(new MongoDBLoggingDataAccess(connection));
    registerDataAccess<data_access::ScriptDataAccess>(new MongoDBScriptDataAccess(connection));
    
    //make needde connection
    getDataAccess<MongoDBNodeDataAccess>()->utility_data_access = getDataAccess<MongoDBUtilityDataAccess>();
    getDataAccess<MongoDBLoggingDataAccess>()->utility_data_access = getDataAccess<MongoDBUtilityDataAccess>();
    getDataAccess<MongoDBScriptDataAccess>()->utility_data_access = getDataAccess<MongoDBUtilityDataAccess>();
    
    getDataAccess<MongoDBUnitServerDataAccess>()->node_data_access = getDataAccess<MongoDBNodeDataAccess>();
    getDataAccess<MongoDBControlUnitDataAccess>()->node_data_access = getDataAccess<MongoDBNodeDataAccess>();
    getDataAccess<MongoDBDataServiceDataAccess>()->node_data_access = getDataAccess<MongoDBNodeDataAccess>();
}

void MongoDBPersistenceDriver::deinit() throw (chaos::CException) {
	connection.reset();
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void MongoDBPersistenceDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
