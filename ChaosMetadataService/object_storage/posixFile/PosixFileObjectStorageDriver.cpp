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

#include "PosixFileObjectStorageDriver.h"
#include "PosixFile.h"

using namespace chaos;

using namespace chaos::service_common::persistence::data_access;

using namespace chaos::metadata_service;

using namespace chaos::metadata_service::object_storage;
using namespace chaos::metadata_service::object_storage::abstraction;

#define INFO    INFO_LOG(PosixFileObjectStorageDriver)
#define DBG     DBG_LOG(PosixFileObjectStorageDriver)
#define ERR     ERR_LOG(PosixFileObjectStorageDriver)

DEFINE_CLASS_FACTORY(PosixFileObjectStorageDriver,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

PosixFileObjectStorageDriver::PosixFileObjectStorageDriver(const std::string& name):
AbstractPersistenceDriver(name){}

PosixFileObjectStorageDriver::~PosixFileObjectStorageDriver() {}

void PosixFileObjectStorageDriver::init(void *init_data) throw (chaos::CException) {
    AbstractPersistenceDriver::init(init_data);
    std::string dir = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param["data"];
    std::string basedatapath;
    if(dir.size()){
        basedatapath=dir;
    } else if(metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->hasOption(InitOption::OPT_DATA_DIR)){
        basedatapath=metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->getOption< std::string>(InitOption::OPT_DATA_DIR);
        
    } else {
        basedatapath=boost::filesystem::current_path().string();
    }
   
   /* const std::string user = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["user"];
    const std::string password = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["pwd"];
    const std::string database = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["db"];
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param;
  */
  if ((boost::filesystem::exists(basedatapath) == false) &&
            (boost::filesystem::create_directories(basedatapath) == false)) {
         ERR<<"cannot create directory:"<<basedatapath;
        throw chaos::CException(-1,__PRETTY_FUNCTION__,"cannot create directory:"+basedatapath);

  }
 
    //register the data access implementations
    registerDataAccess<ObjectStorageDataAccess>(new PosixFile(basedatapath));
}

void PosixFileObjectStorageDriver::deinit() throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void PosixFileObjectStorageDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
