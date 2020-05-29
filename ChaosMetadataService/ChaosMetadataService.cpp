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

#include "mds_constants.h"
#include "ChaosMetadataService.h"
#include "DriverPoolManager.h"
#include "QueryDataConsumer.h"
#include "object_storage/object_storage.h"

#include <csignal>
#include <chaos/common/exception/CException.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/io/SharedManagedDirecIoDataDriver.h>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
using namespace std;
using namespace chaos;
using namespace chaos::common::io;

using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service::cache_system;

using namespace chaos::metadata_service;

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;
using namespace chaos::common::healt_system;

using namespace chaos::service_common::persistence::data_access;

WaitSemaphore ChaosMetadataService::waitCloseSemaphore;
uint64_t ChaosMetadataService::timePrecisionMask=0xFFFFFFFFFFFFFFF0ULL;

#define LCND_LAPP   INFO_LOG(ChaosMetadataService)
#define LCND_LDBG   DBG_LOG(ChaosMetadataService)
#define LCND_LERR   ERR_LOG(ChaosMetadataService)
#define log(x) LCND_LDBG<<x
ChaosMetadataService::ChaosMetadataService(){ingore_unreg_po = true;
};
ChaosMetadataService::~ChaosMetadataService(){}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosMetadataService::init(int argc, const char* argv[])  {
    ChaosCommon<ChaosMetadataService>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosMetadataService::init(istringstream &initStringStream)  {
    ChaosCommon<ChaosMetadataService>::init(initStringStream);
}

/*
 *
 */
void ChaosMetadataService::init(void *init_data)  {
    try {
        ChaosCommon<ChaosMetadataService>::init(init_data);
        
        if (signal((int) SIGINT, ChaosMetadataService::signalHanlder) == SIG_ERR) {
            throw CException(-1, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
        }
        
        if (signal((int) SIGQUIT, ChaosMetadataService::signalHanlder) == SIG_ERR) {
            throw CException(-2, "Error registering SIG_ERR signal", __PRETTY_FUNCTION__);
        }
        
        if (signal((int) SIGTERM, ChaosMetadataService::signalHanlder) == SIG_ERR) {
            throw CException(-3, "Error registering SIGTERM signal", __PRETTY_FUNCTION__);
        }
        
        //scan the setting
        if(!setting.persistence_implementation.size()) {
            //no cache server provided
            throw chaos::CException(-3, "No persistence implementation provided", __PRETTY_FUNCTION__);
        }
        
        if(!setting.persistence_server_list.size()) {
            //no cache server provided
            throw chaos::CException(-4, "No persistence's server list provided", __PRETTY_FUNCTION__);
        }
        
        if(getGlobalConfigurationInstance()->hasOption(OPT_PERSITENCE_KV_PARAMTER)) {
            fillKVParameter(setting.persistence_kv_param_map,
                            getGlobalConfigurationInstance()->getOption< std::vector< std::string> >(OPT_PERSITENCE_KV_PARAMTER));
        }
        
        //check for mandatory configuration
        if(!getGlobalConfigurationInstance()->hasOption(OPT_CACHE_SERVER_LIST)) {
            //no cache server provided
            throw chaos::CException(-3, "No cache server provided", __PRETTY_FUNCTION__);
        }
        
        if(getGlobalConfigurationInstance()->hasOption(OPT_SYNCTIME_ERROR)) {
           timeError_opt= getGlobalConfigurationInstance()->getOption< uint32_t >(OPT_SYNCTIME_ERROR);

        }

        if(timeError_opt>2048){
            std::stringstream ss;
            ss<<"Specified time synchronization error to high:"<<timeError_opt<<" ms";
            throw chaos::CException(-4,ss.str(), __PRETTY_FUNCTION__);

        }
        uint64_t tmp=pow(2,(uint32_t)log2(timeError_opt));
        timePrecisionMask=~(tmp-1);
        if(getGlobalConfigurationInstance()->hasOption(OPT_CACHE_DRIVER_KVP)) {
            GlobalConfiguration::getInstance()->fillKVParameter(setting.cache_driver_setting.key_value_custom_param,
                                                                getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP), "");
//            fillKVParameter(setting.cache_driver_setting.key_value_custom_param,
//                            getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP));
        }
        
        if(getGlobalConfigurationInstance()->hasOption(OPT_OBJ_STORAGE_DRIVER_KVP)) {
            GlobalConfiguration::getInstance()->fillKVParameter(setting.object_storage_setting.key_value_custom_param,
                                                                getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_OBJ_STORAGE_DRIVER_KVP), "");
//            fillKVParameter(setting.object_storage_setting.key_value_custom_param,
//                            getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_OBJ_STORAGE_DRIVER_KVP));
        }
         if(getGlobalConfigurationInstance()->hasOption(OPT_LOG_STORAGE_DRIVER_KVP)) {
            GlobalConfiguration::getInstance()->fillKVParameter(setting.log_storage_setting.key_value_custom_param,
                                                                getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_LOG_STORAGE_DRIVER_KVP), "");
//            fillKVParameter(setting.object_storage_setting.key_value_custom_param,
//                            getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_OBJ_STORAGE_DRIVER_KVP));
        }
        //initilize driver pool manager
        InizializableService::initImplementation(DriverPoolManager::getInstance(), NULL, "DriverPoolManager", __PRETTY_FUNCTION__);
        
        //! batch system
        StartableService::initImplementation(MDSBatchExecutor::getInstance(), NULL, "MDSBatchExecutor", __PRETTY_FUNCTION__);
        
        //api system
        api_managment_service.reset(new ApiManagment(), "ApiManagment");
        api_managment_service.init(NULL, __PRETTY_FUNCTION__);
        
        data_consumer.reset(new QueryDataConsumer(), "QueryDataConsumer");
        if(!data_consumer.get()) throw chaos::CException(-7, "Error instantiating data consumer", __PRETTY_FUNCTION__);
        data_consumer.init(NULL, __PRETTY_FUNCTION__);
        
        //initialize cron manager
        InizializableService::initImplementation(cron_job::MDSCronusManager::getInstance(),
                                                 NULL,
                                                 "MDSConousManager",
                                                 __PRETTY_FUNCTION__);

        InizializableService::initImplementation(SharedManagedDirecIoDataDriver::getInstance(), NULL, "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);

        StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealthManager", __PRETTY_FUNCTION__);

        
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //start data manager
}

/*
 *
 */
void ChaosMetadataService::start()  {
    //lock o monitor for waith the end
    try {
        ChaosCommon<ChaosMetadataService>::start();
        StartableService::startImplementation(MDSBatchExecutor::getInstance(), "MDSBatchExecutor", __PRETTY_FUNCTION__);
        //start batch system
        data_consumer.start( __PRETTY_FUNCTION__);
        LAPP_ <<"\n----------------------------------------------------------------------"<<
        "\n!CHAOS Metadata service started" <<
        "\nRPC Server address: "	<< NetworkBroker::getInstance()->getRPCUrl() <<
        "\nDirectIO Server address: " << NetworkBroker::getInstance()->getDirectIOUrl() <<
        CHAOS_FORMAT("\nData Service published with url: %1%|0", %NetworkBroker::getInstance()->getDirectIOUrl()) <<
        "\nTime precision mask: "<<std::hex<<timePrecisionMask<<std::dec<<"\n----------------------------------------------------------------------";
        
        //register this process on persistence database
        persistence::data_access::DataServiceDataAccess *ds_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<persistence::data_access::DataServiceDataAccess>();
        std::string unique_uid=NetworkBroker::getInstance()->getRPCUrl();
        ds_da->registerNode(setting.ha_zone_name,
                            unique_uid,
                            NetworkBroker::getInstance()->getDirectIOUrl(),
                            0);

        //at this point i must with for end signal
        chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this,
                                                                                   0,
                                                                                   chaos::common::constants::HBTimersTimeoutinMSec);


        StartableService::startImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__);
        HealtManager::getInstance()->addNewNode(unique_uid);
        HealtManager::getInstance()->addNodeMetricValue(unique_uid,
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_START);
            
        waitCloseSemaphore.wait();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    //execute the deinitialization of CU
    try{
        stop();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    
    try{
        deinit();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
}

void ChaosMetadataService::timeout() {
    int err = 0;
    bool presence = false;
    HealthStat service_proc_stat;
    const std::string ds_uid = NetworkBroker::getInstance()->getRPCUrl();
    persistence::data_access::DataServiceDataAccess *ds_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<persistence::data_access::DataServiceDataAccess>();
    persistence::data_access::NodeDataAccess *n_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<persistence::data_access::NodeDataAccess>();
    service_proc_stat.mds_received_timestamp = TimingUtil::getTimeStamp();
    if(n_da->checkNodePresence(presence, ds_uid) != 0) {
        LCND_LERR << CHAOS_FORMAT("Error check if this mds [%1%] description is registered", %NetworkBroker::getInstance()->getRPCUrl());
        return;
    }

    if(presence == false) {
        //reinsert mds
        ds_da->registerNode(setting.ha_zone_name,
                            NetworkBroker::getInstance()->getRPCUrl(),
                            NetworkBroker::getInstance()->getDirectIOUrl(),
                            0);
    }

    //update proc stat
    ProcStatCalculator::update(service_proc_stat);
    if((err = n_da->setNodeHealthStatus(NetworkBroker::getInstance()->getRPCUrl(),
                                        service_proc_stat))) {
        LCND_LERR << CHAOS_FORMAT("error storing health data into database for this mds [%1%]", %NetworkBroker::getInstance()->getRPCUrl());
    }
}

bool ChaosMetadataService::isNodeAlive(const std::string& uid){
    ChaosStringVector s;
    s.push_back(uid);
    bool alive=areNodeAlive(s)[0];
    //LCND_LDBG<<"NODE:"<<uid<<" "<<((alive)?"TRUE":"FALSE");
    return alive;
    }
using namespace chaos::metadata_service::object_storage::abstraction;
using namespace chaos::metadata_service::persistence::data_access;

int ChaosMetadataService::removeStorageData(const std::string& control_unit_found,uint64_t start,uint64_t remove_until_ts){
    int err;
    auto *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<ObjectStorageDataAccess>();

    if(obj_storage_da==NULL ){
        LCND_LERR<< " cannot access object storage resources";
        return -1;
    }
    LCND_LDBG<<" deleting node storage "<<control_unit_found<<" from:"<<start<<" to:"<<remove_until_ts;
      const std::string output_key    = control_unit_found + DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
        const std::string input_key     = control_unit_found + DataPackPrefixID::INPUT_DATASET_POSTFIX;
        const std::string system_key    = control_unit_found + DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
        const std::string custom_key    = control_unit_found + DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
        const std::string health_key    = control_unit_found + NodeHealtDefinitionKey::HEALT_KEY_POSTFIX;
        const std::string dev_alarm_key    = control_unit_found + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
        const std::string cu_alarm_key    = control_unit_found + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
      
       try {
            log(CHAOS_FORMAT("Remove data for key %1%", %output_key));
            
            if((err = obj_storage_da->deleteObject(output_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %output_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %input_key));
            if((err = obj_storage_da->deleteObject(input_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %input_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %system_key));
            if((err = obj_storage_da->deleteObject(system_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %system_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %custom_key));
            if((err = obj_storage_da->deleteObject(custom_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %custom_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %health_key));
            if((err = obj_storage_da->deleteObject(health_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %health_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %dev_alarm_key));
            if((err = obj_storage_da->deleteObject(dev_alarm_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %dev_alarm_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %cu_alarm_key));
            if((err = obj_storage_da->deleteObject(cu_alarm_key,
                                                   start,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %cu_alarm_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove log for cu %1%", %control_unit_found));
            if((err = DriverPoolManager::getInstance()->getPersistenceDataAccess<persistence::data_access::LoggingDataAccess>()->eraseLogBeforTS(control_unit_found,
                                                                                                    remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing logging for control unit %1% with error %2%", %control_unit_found%err));
            }
            }catch(CException& ex){
            log(ex.what());
            return -100;
        }catch(...){
            log("Undeterminated error during ageing management");
            return -200;
        }
    return err;

}

std::vector<bool> ChaosMetadataService::areNodeAlive(const ChaosStringVector& uids){
        int err=0;
        std::vector<bool> res;
        CacheDriver& cache_slot = DriverPoolManager::getInstance()->getCacheDrv();
        DataBuffer data_buffer;
        MultiCacheData multi_cached_data;
        ChaosStringVector keys;
        for(ChaosStringVector::const_iterator i=uids.begin();i!=uids.end();i++){
            keys.push_back((*i)+NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
        }
        if(keys.size()==0) return res;
        err = cache_slot.getData(keys,
                                 multi_cached_data);
        uint64_t now=chaos::common::utility::TimingUtil::getTimeStamp();
        for(ChaosStringVectorConstIterator it = keys.begin(),
            end = keys.end();
            it != end;
            it++) {
            const CacheData& cached_element = multi_cached_data[*it];
            if(!cached_element ||
               cached_element->size() == 0) {
                res.push_back(false);
            } else {
                CDataWrapper ca(cached_element->data(),cached_element->size());
                uint64_t ts=0;
                if(ca.hasKey(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP)){
                    ts=ca.getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);
                } else if(ca.hasKey(DataPackCommonKey::DPCK_TIMESTAMP)){
                    ts=ca.getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
                }
                 res.push_back((ts>(now-(2*chaos::common::constants::HBTimersTimeoutinMSec))));

            }
        }
        return res;
    
}

/*
 Stop the toolkit execution
 */
void ChaosMetadataService::stop() {
    CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__););

    chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);
    
    //stop data consumer
    data_consumer.stop( __PRETTY_FUNCTION__);
    
    StartableService::stopImplementation(MDSBatchExecutor::getInstance(), "MDSBatchExecutor", __PRETTY_FUNCTION__);
    
    ChaosCommon<ChaosMetadataService>::stop();
    //endWaithCondition.notify_one();
    waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosMetadataService::deinit() {
    InizializableService::deinitImplementation(SharedManagedDirecIoDataDriver::getInstance(), "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);

    CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__););

    InizializableService::deinitImplementation(cron_job::MDSCronusManager::getInstance(),
                                               "MDSConousManager",
                                               __PRETTY_FUNCTION__);
    //deinit api system
    CHAOS_NOT_THROW(api_managment_service.deinit(__PRETTY_FUNCTION__);)
    
    if(data_consumer.get()) {
        data_consumer.deinit(__PRETTY_FUNCTION__);
    }
    
    StartableService::deinitImplementation(MDSBatchExecutor::getInstance(), "MDSBatchExecutor", __PRETTY_FUNCTION__);
    
    //deinitilize driver pool manager
    InizializableService::deinitImplementation(DriverPoolManager::getInstance(), "DriverPoolManager", __PRETTY_FUNCTION__);
    
    ChaosCommon<ChaosMetadataService>::stop();
    LAPP_ << "-----------------------------------------";
    LAPP_ << "Metadata service has been stopped";
    LAPP_ << "-----------------------------------------";
}

/*
 *
 */
void ChaosMetadataService::signalHanlder(int signalNumber) {
    waitCloseSemaphore.unlock();
}

void ChaosMetadataService::fillKVParameter(std::map<std::string, std::string>& kvmap,
                                           const std::vector<std::string>& multitoken_param) {
    //! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
    boost::regex KVParamRegex("[a-zA-Z0-9/_-]+:[a-zA-Z0-9/_-]+");
    std::vector<std::string> kv_splitted;
    std::vector<std::string> kvtokens;
    for(std::vector<std::string>::const_iterator it = multitoken_param.begin();
        it != multitoken_param.end();
        it++) {
        
        const std::string& param_key = *it;
        
        
        
        if(!regex_match(param_key, KVParamRegex)) {
            throw chaos::CException(-3, "Malformed kv parameter string", __PRETTY_FUNCTION__);
        }
        
        
        boost::algorithm::split(kvtokens,
                                param_key,
                                boost::algorithm::is_any_of("-"),
                                boost::algorithm::token_compress_on);
        
        //clear previosly pair
        kv_splitted.clear();
        
        //get new pair
        boost::algorithm::split(kv_splitted,
                                param_key,
                                boost::algorithm::is_any_of(":"),
                                boost::algorithm::token_compress_on);
        // add key/value pair
        kvmap.insert(std::pair<std::string,std::string>(kv_splitted[0], kv_splitted[1]));
    }
}
