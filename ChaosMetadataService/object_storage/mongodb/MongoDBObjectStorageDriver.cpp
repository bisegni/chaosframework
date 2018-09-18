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

#include "../../ChaosMetadataService.h"

#include "MongoDBObjectStorageDriver.h"

#include "MongoDBObjectStorageDataAccess.h"

using namespace chaos;

using namespace chaos::service_common::persistence::data_access;
using namespace chaos::service_common::persistence::mongodb;

using namespace chaos::metadata_service;

using namespace chaos::metadata_service::object_storage::mongodb;
using namespace chaos::metadata_service::object_storage::abstraction;

DEFINE_CLASS_FACTORY(MongoDBObjectStorageDriver,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

MongoDBObjectStorageDriver::MongoDBObjectStorageDriver(const std::string& name):
AbstractPersistenceDriver(name){}

MongoDBObjectStorageDriver::~MongoDBObjectStorageDriver() {}

void MongoDBObjectStorageDriver::init(void *init_data)  {
    //call sublcass
    AbstractPersistenceDriver::init(init_data);
    
    
    //we can configura the connection
    connection.reset(new MongoDBHAConnectionManager(ChaosMetadataService::getInstance()->setting.object_storage_setting.url_list,
                                                    ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param));
    
    //register the data access implementations
    registerDataAccess<ObjectStorageDataAccess>(new MongoDBObjectStorageDataAccess(connection));
}

void MongoDBObjectStorageDriver::deinit()  {
    //call sublcass
    AbstractPersistenceDriver::deinit();
    
    connection.reset();
}

void MongoDBObjectStorageDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
