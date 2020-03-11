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
#include <boost/algorithm/string.hpp>

#include "InfluxDBLogStorageDriver.h"
#include "InfluxDB.h"
using namespace chaos;

using namespace chaos::service_common::persistence::data_access;

using namespace chaos::metadata_service;

using namespace chaos::metadata_service::object_storage;
using namespace chaos::metadata_service::object_storage::abstraction;

#define INFO    INFO_LOG(InfluxDBLogStorageDriver)
#define DBG     DBG_LOG(InfluxDBLogStorageDriver)
#define ERR     ERR_LOG(InfluxDBLogStorageDriver)

DEFINE_CLASS_FACTORY(InfluxDBLogStorageDriver,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

InfluxDBLogStorageDriver::InfluxDBLogStorageDriver(const std::string& name):
AbstractPersistenceDriver(name){}

InfluxDBLogStorageDriver::~InfluxDBLogStorageDriver() {}

void InfluxDBLogStorageDriver::init(void *init_data) throw (chaos::CException) {
    AbstractPersistenceDriver::init(init_data);

    const ChaosStringVector url_list = ChaosMetadataService::getInstance()->setting.log_storage_setting.url_list;
    const std::string user = ChaosMetadataService::getInstance()->setting.log_storage_setting.key_value_custom_param["user"];
    const std::string password = ChaosMetadataService::getInstance()->setting.log_storage_setting.key_value_custom_param["pwd"];
    const std::string database = ChaosMetadataService::getInstance()->setting.log_storage_setting.key_value_custom_param["db"];
   
    std::string dir;
   
    MapKVP& obj_storage_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
   
    std::string basedatapath;
    if(dir.size()){
        basedatapath=dir;
    } else if(metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->hasOption(InitOption::OPT_DATA_DIR)){
        basedatapath=metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->getOption< std::string>(InitOption::OPT_DATA_DIR);
        
    } else {
        basedatapath=boost::filesystem::current_path().string();
    }
    std::string servername="localhost";
    int port=8086;
    if(url_list.size()>0){
        std::vector<std::string> ele;
        boost::split(ele,url_list[0],boost::is_any_of(":"));
        if(ele.size()>0){
            servername=ele[0];
        }
        if(ele.size()>1){
            port=atoi(ele[1].c_str());
        }
    }
    if(database.size()==0){
        ERR<<"You must specify a valid database name";
        throw chaos::CException(-1,"You must specify a valid database name",__FUNCTION__);
    }
    //influxdb_t  asyncdb = influxdb_t( new influxdb::async_api::simple_db(url_list[0], database));
   // asyncdb->with_authentication(user,password);
    DBG<<"server:"<<servername<<"\nport:"<<port<<"\ndatabase:"<<database<<"\nuser:"<<user<<"\npassw:"<<password;
    influxdb_cpp::server_info si(servername,port,database,user,password,"ms","365d");
    //register the data access implementations
    std::string resp;
    int ret;
    if((ret=influxdb_cpp::show_db(resp,si))<0){
       ERR<<"cannot show DB:"<<database<< " on:"<<servername<<" port:"<<port;
        throw chaos::CException(ret,"cannot connect or create DB:"+database+" on server:"+servername,__FUNCTION__);  
    }
    CDataWrapper r;
    r.setSerializedJsonData(resp.c_str());
    DBG<<" DB returned:"<<ret<<" answer:\""<<resp<<"\"";

    if((ret=influxdb_cpp::create_db(resp,database,si))<0){
       ERR<<"cannot connect or create DB:"<<database<< " on:"<<servername<<" port:"<<port;
    throw chaos::CException(ret,"cannot connect or create DB:"+database+" on server:"+servername,__FUNCTION__);  
    }
    r.setSerializedJsonData(resp.c_str());
    DBG<<" DB returned:"<<ret<<" answer:\""<<resp<<"\"";

    registerDataAccess<ObjectStorageDataAccess>(new InfluxDB(si));
}

void InfluxDBLogStorageDriver::deinit() throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void InfluxDBLogStorageDriver::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}
