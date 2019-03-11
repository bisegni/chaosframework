/*
 * Copyright 2012, 25/05/2018 INFN
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

#include "CassHybObjectStorageDriver.h"
#include "HybCassDataAccess.h"

#include "../../../ChaosMetadataService.h"

#define INFO    INFO_LOG(CassHybObjectStorageDriver)
#define DBG     DBG_LOG(CassHybObjectStorageDriver)
#define ERR     ERR_LOG(CassHybObjectStorageDriver)


using namespace chaos::metadata_service::object_storage::abstraction;

using namespace chaos::metadata_service::object_storage::hybdriver;
using namespace chaos::metadata_service::object_storage::hybdriver::cassandra;

chaos::common::utility::ObjectFactoryInstancer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver>
CassHybObjectStorageDriverObjectFactoryInstancer(new chaos::common::utility::ObjectFactoryAliasInstantiation<CassHybObjectStorageDriver>("CassHybObjectStorageDriver"));

CassHybObjectStorageDriver::CassHybObjectStorageDriver(const std::string& name):
HybBaseDriver(name){}

CassHybObjectStorageDriver::~CassHybObjectStorageDriver(){}

void CassHybObjectStorageDriver::init(void *init_data) throw (chaos::CException) {
    CassError err = CASS_OK;
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    
    ChaosStringVector url_list = ChaosMetadataService::getInstance()->setting.object_storage_setting.url_list;
    const std::string user = obj_stoarge_kvp["cass_user"];
    const std::string password = obj_stoarge_kvp["cass_pwd"];
    const std::string keyspace = obj_stoarge_kvp["cass_db"];
    const std::string url = obj_stoarge_kvp["cass_url"];
    
    cluster_shrd_ptr = ALLOCATE_CLUSTER();
    if((err = cass_cluster_set_contact_points(cluster_shrd_ptr.get(), url.c_str())) != CASS_OK) {
        LOG_AND_TROW(ERR, -1, cass_error_desc(err))
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
    
    
    HybBaseDriver::init(init_data);
}

void CassHybObjectStorageDriver::deinit() throw (chaos::CException) {
     HybBaseDriver::deinit();
}

HybBaseDataAccess *CassHybObjectStorageDriver::dataAccessImpl() {
    ChaosUniquePtr<HybBaseDataAccess> da(new HybCassDataAccess(session_shrd_ptr));
    return da.release();
}
