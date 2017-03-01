/*
 *	main.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <iostream>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include "ChaosMetadataService.h"

using namespace chaos::metadata_service;

int main(int argc, char * argv[]) {
    try {
        //data worker
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_DATA_WORKER_LOG_METRIC,
                                                                                                 "Enable log metric for data worker",
                                                                                                 false,
                                                                                                 &ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_DATA_WORKER_LOG_METRIC_UPDATE_INTERVAL,
                                                                                                     "Specify the data worker log interval in second",
                                                                                                     1,
                                                                                                     &ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric_update_interval);
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_DATA_WORKER_NUM,
                                                                                                         "The number of the data worker",
                                                                                                         DATA_WORKER_NUMBER,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_DATA_WORKER_THREAD,
                                                                                                         "The thread number of each data worker",
                                                                                                         1,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_setting.job_thread_number);
        
        //cache parameter
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_CACHE_DRIVER,
                                                                                                        "Cache driver implementation",
                                                                                                        "Couchbase",
                                                                                                        &ChaosMetadataService::getInstance()->setting.cache_driver_setting.cache_driver_impl);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_SERVER_LIST,
                                                                                                                     "The list of the cache server",
                                                                                                                     &ChaosMetadataService::getInstance()->setting.cache_driver_setting.startup_chache_servers);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_CACHE_DRIVER_POOL_MIN_INSTANCE,
                                                                                                     "The minimum number of cache driver instance sin the pool",
                                                                                                     3,
                                                                                                     &ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_pool_min_instances_number);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP,
                                                                                                                     "The key value parameter for cache implementation driver (ex k:v-k1:v1)");
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_CACHE_LOG_METRIC,
                                                                                                 "Enable log metric for cache driver",
                                                                                                 false,
                                                                                                 &ChaosMetadataService::getInstance()->setting.cache_driver_setting.log_metric);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_CACHE_LOG_METRIC_UPDATE_INTERVAL,
                                                                                                     "Specify the cache metric log interval in second",
                                                                                                     1,
                                                                                                     &ChaosMetadataService::getInstance()->setting.cache_driver_setting.log_metric_update_interval);
        
        //object storage
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_OBJ_STORAGE_DRIVER,
                                                                                                        "Object storage driver implementation",
                                                                                                        "MongoDB",
                                                                                                        &ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_OBJ_STORAGE_SERVER_URL,
                                                                                                                     "The list of the obj storage servers",
                                                                                                                     &ChaosMetadataService::getInstance()->setting.object_storage_setting.url_list);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_OBJ_STORAGE_DRIVER_KVP,
                                                                                                                     "The key value multitoken for obj storage implementation driver (k:v)");
        
        //persistence
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_BATCH_SANDBOX_SIZE,
                                                                                                         "Specify number of the sanbox to use into the batch subsytem",
                                                                                                         &ChaosMetadataService::getInstance()->setting.batch_sandbox_size);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_PERSITENCE_IMPL,
                                                                                                        "Specify the implementation of the persistence layer",
                                                                                                        &ChaosMetadataService::getInstance()->setting.persistence_implementation);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_PERSITENCE_SERVER_ADDR_LIST,
                                                                                                                     "Specify servers where the persistence layer needs to use",
                                                                                                                     &ChaosMetadataService::getInstance()->setting.persistence_server_list);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_PERSITENCE_KV_PARAMTER,
                                                                                                                     "The key value parameter for storage implementation driver (ex k:v-k1:v1)");
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CRON_JOB_CHECK,
                                                                                                         "Specify the check time (in seconds ) for the cron job scheduler",
                                                                                                         3600,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cron_job_scheduler_repeat_time);
        
        ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CRON_JOB_AGEING_MANAGEMENT,
                                                                                                         "Specify the check time (in seconds ) for ageing management (default is one day of delay)",
                                                                                                         86400,
                                                                                                         &ChaosMetadataService::getInstance()->setting.cron_job_ageing_management_repeat_time);
        ChaosMetadataService::getInstance()->init(argc, argv);
        
        ChaosMetadataService::getInstance()->start();
    } catch (chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    CHAOS_NOT_THROW(ChaosMetadataService::getInstance()->stop(););
    CHAOS_NOT_THROW(ChaosMetadataService::getInstance()->deinit(););
    return 0;
}

