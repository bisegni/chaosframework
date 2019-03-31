/*
 * Copyright 2012, 18/06/2018 INFN
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

//#include <chaos/common/global.h>

#include "../../ChaosMetadataService.h"

#include "HybBaseDriver.h"
#include "HybBaseDataAccess.h"

using namespace chaos;

using namespace chaos::service_common::persistence::data_access;

using namespace chaos::metadata_service;


using namespace chaos::metadata_service::object_storage;
using namespace chaos::metadata_service::object_storage::hybdriver;
using namespace chaos::metadata_service::object_storage::abstraction;

#define INFO    INFO_LOG(HybBaseDriver)
#define DBG     DBG_LOG(HybBaseDriver)
#define ERR     ERR_LOG(HybBaseDriver)

HybBaseDriver::HybBaseDriver(const std::string& name):
AbstractPersistenceDriver(name),
BaseMongoDBDiver(){}

HybBaseDriver::~HybBaseDriver() {}

void HybBaseDriver::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    ChaosStringVector url_list = ChaosMetadataService::getInstance()->setting.object_storage_setting.url_list;
    const std::string user = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["user"];
    const std::string password = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["pwd"];
    const std::string database = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["db"];
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    
    if(obj_stoarge_kvp.count("mongodb_oswc")) {
        //set the custom write concern
        INFO << CHAOS_FORMAT("Set MongoDB object storage write concern to %1%", %obj_stoarge_kvp["mongodb_oswc"]);
        if(obj_stoarge_kvp["mongodb_oswc"].compare("unacknowledged") == 0) {
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("acknowledged") == 0) {
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("journaled") == 0) {
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("replicated") == 0) {
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("majority") == 0) {
        } else {
            //LOG_AND_THROW(ERR, -1, CHAOS_FORMAT("Unrecognized value for parameter mongodb_oswc[%1%]", %obj_stoarge_kvp["mongodb_oswc"]));
        }
    }
    if(obj_stoarge_kvp.count("maxPoolSize")) {
    }
    if(obj_stoarge_kvp.count("minPoolSize")) {
    }
    //initilize pool
    BaseMongoDBDiver::initPool(url_list, user, password, database);
    
    //register the data access implementations
    HybBaseDataAccess *acc = dataAccessImpl();
    acc->pool_ref = BaseMongoDBDiver::getSharedPool();
    registerDataAccess<ObjectStorageDataAccess>(acc);
}

void HybBaseDriver::deinit() throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void HybBaseDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
