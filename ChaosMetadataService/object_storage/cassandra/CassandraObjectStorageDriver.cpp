/*
 * Copyright 2012, 29/11/2018 INFN
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

#if USE_CASSANDRA_DRIVER
#include "../../ChaosMetadataService.h"

#include "CassandraObjectStorageDriver.h"
#include "CassandraObjectStorageDataAccess.h"

//#include "MongoDBObjectStorageDataAccess.h"
using namespace chaos;
using namespace chaos::metadata_service;
using namespace chaos::metadata_service::object_storage;
using namespace chaos::metadata_service::object_storage::cassandra;
using namespace chaos::metadata_service::object_storage::abstraction;

#define INFO INFO_LOG(CassandraObjectStorageDriver)
#define DBG  DBG_LOG(CassandraObjectStorageDriver)
#define ERR  ERR_LOG(CassandraObjectStorageDriver)

DEFINE_CLASS_FACTORY(CassandraObjectStorageDriver,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

CassandraObjectStorageDriver::CassandraObjectStorageDriver(const std::string& name):
AbstractPersistenceDriver(name){}

CassandraObjectStorageDriver::~CassandraObjectStorageDriver() {}

void CassandraObjectStorageDriver::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    CassError err = CASS_OK;
    AbstractPersistenceDriver::init(init_data);
    ChaosStringVector url_list = ChaosMetadataService::getInstance()->setting.object_storage_setting.url_list;
    const std::string user = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["user"];
    const std::string password = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["pwd"];
    const std::string keyspace = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["db"];
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    
    cluster_shrd_ptr = ALLOCATE_CLUSTER();
    for (std::string url : url_list) {
        if((err = cass_cluster_set_contact_points(cluster_shrd_ptr.get(), url.c_str())) != CASS_OK) {
            //error
            LOG_AND_TROW(ERR, -1, cass_error_desc(err))
        }
    }
    
    if(user.size() &&
       password.size()) {
        cass_cluster_set_credentials(cluster_shrd_ptr.get(),
                                     user.c_str(),
                                     password.c_str());
    }
    
    session_shrd_ptr = ALLOCATE_SESSION();
    MAKE_MANAGED_FUTURE(connect_future, cass_session_connect_keyspace(session_shrd_ptr.get(),
                                                                      cluster_shrd_ptr.get(),
                                                                      keyspace.c_str()));
    //wait for connection
    if ((err = cass_future_error_code(connect_future.get())) != CASS_OK) {
        LOG_AND_TROW(ERR, -2, cass_error_desc(err))
    }

    
    //register the data access implementations
    registerDataAccess<ObjectStorageDataAccess>(new CassandraObjectStorageDataAccess(session_shrd_ptr));
}

void CassandraObjectStorageDriver::deinit() throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::deinit();
    
}

void CassandraObjectStorageDriver::deleteDataAccess(void *instance) {
    abstraction::ObjectStorageDataAccess::AbstractDataAccess *da_instance = static_cast<abstraction::ObjectStorageDataAccess::AbstractDataAccess*>(instance);
    delete(da_instance);
}

#endif
