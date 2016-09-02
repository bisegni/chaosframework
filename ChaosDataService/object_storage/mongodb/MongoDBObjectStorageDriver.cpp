/*
 *	MongoDBObjectStorageDriver.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#include "../../ChaosDataService.h"

#include "MongoDBObjectStorageDriver.h"

#include "MongoDBObjectStorageDataAccess.h"

using namespace chaos;

using namespace chaos::service_common::persistence::data_access;
using namespace chaos::service_common::persistence::mongodb;

using namespace chaos::data_service::object_storage::mongodb;
using namespace chaos::data_service::object_storage::abstraction;

DEFINE_CLASS_FACTORY(MongoDBObjectStorageDriver,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

MongoDBObjectStorageDriver::MongoDBObjectStorageDriver(const std::string& name):
AbstractPersistenceDriver(name){}

MongoDBObjectStorageDriver::~MongoDBObjectStorageDriver() {}

void MongoDBObjectStorageDriver::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::init(init_data);
    
    
    
    //we can configura the connection
    connection.reset(new MongoDBHAConnectionManager(ChaosDataService::getInstance()->setting.object_storage_setting.url_list,
                                                    ChaosDataService::getInstance()->setting.object_storage_setting.key_value_custom_param));
    
    //register the data access implementations
    registerDataAccess<ObjectStorageDataAccess>(new MongoDBObjectStorageDataAccess(connection));
}

void MongoDBObjectStorageDriver::deinit() throw (chaos::CException) {
    connection.reset();
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void MongoDBObjectStorageDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
