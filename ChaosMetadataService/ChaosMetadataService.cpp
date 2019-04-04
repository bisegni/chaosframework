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

#include <csignal>
#include <chaos/common/exception/CException.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
using namespace std;
using namespace chaos;

using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::data::structured;

using namespace chaos::metadata_service;

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;

using namespace chaos::service_common::persistence::data_access;

WaitSemaphore ChaosMetadataService::waitCloseSemaphore;

#define LCND_LAPP   INFO_LOG(ChaosMetadataService)
#define LCND_LDBG   DBG_LOG(ChaosMetadataService)
#define LCND_LERR   ERR_LOG(ChaosMetadataService)
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
        "\n----------------------------------------------------------------------";
        
        //register this process on persistence database
        persistence::data_access::DataServiceDataAccess *ds_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<persistence::data_access::DataServiceDataAccess>();

        ds_da->registerNode(setting.ha_zone_name,
                            NetworkBroker::getInstance()->getRPCUrl(),
                            NetworkBroker::getInstance()->getDirectIOUrl(),
                            0);

        //at this point i must with for end signal
        chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this,
                                                                                   0,
                                                                                   chaos::common::constants::HBTimersTimeoutinMSec);
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

/*
 Stop the toolkit execution
 */
void ChaosMetadataService::stop() {
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
