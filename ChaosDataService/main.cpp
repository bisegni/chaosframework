/*
 *	main.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "ChaosDataService.h"

#include <vector>
#include <exception>

using namespace chaos::data_service;


int main(int argc, char * argv[]) {
    try {
		std::vector<std::string> cache_servers;
		
		//cache parameter
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_CACHE_ONLY,
																							 "Specify if we need to enable only the caching feature (no history managment)",
																							 false,
																							 &ChaosDataService::getInstance()->setting.cache_only);

		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_RUN_MODE,
																									 "Specify the run mode[1 - Query Consumer, 2 - Stage Indexer, 3 - Both]");
		
		//cache parameter
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_CACHE_LOG_METRIC,
                                                                                             "Enable log metric for cache driver",
                                                                                             false,
                                                                                             &ChaosDataService::getInstance()->setting.cache_driver_setting.log_metric);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_CACHE_LOG_METRIC_UPDATE_INTERVAL,
                                                                                             "Specify the cache metric log interval in second",
                                                                                             1,
                                                                                             &ChaosDataService::getInstance()->setting.cache_driver_setting.log_metric_update_interval);

		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_CACHE_DRIVER,
																									"Cache driver implementation",
																									"Couchbase",
																									&ChaosDataService::getInstance()->setting.cache_driver_setting.cache_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_SERVER_LIST,
																												 "The list of the cache server",
																												 &ChaosDataService::getInstance()->setting.cache_driver_setting.startup_chache_servers);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_CACHE_DRIVER_POOL_MIN_INSTANCE,
                                                                                                "The minimum number of cache driver instance sin the pool",
                                                                                                 3,
                                                                                                 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_pool_min_instances_number);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_DATA_WORKER_LOG_METRIC,
                                                                                             "Enable log metric for data worker",
                                                                                             false,
                                                                                             &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_DATA_WORKER_LOG_METRIC_UPDATE_INTERVAL,
                                                                                                 "Specify the data worker log interval in second",
                                                                                                 1,
                                                                                                 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric_update_interval);
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_DATA_WORKER_NUM,
																									 "The number of the data worker",
																									 DATA_WORKER_NUMBER,
																									 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_DATA_WORKER_THREAD,
																								 "The thread number of each data worker",
																								 1,
																								 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_setting.job_thread_number);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP,
																												 "The key value parameter for cache implementation driver (ex k:v-k1:v1)");
		
		/*
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_INDEXER_WORKER_NUM,
																								 "The number of the indexer worker",
																								 INDEXER_DEFAULT_WORKER_NUMBER,
																								 &ChaosDataService::getInstance()->setting.indexer_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_INDEXER_SCAN_DELAY,
																								 "The repeat delay for virtual system scan in seconds",
																								 INDEXER_DEFAULT_SCAN_DELAY,
																								 &ChaosDataService::getInstance()->setting.indexer_scan_delay);
         */
        //object storage
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_OBJ_STORAGE_DRIVER,
                                                                                                    "Object storage driver implementation",
                                                                                                    "MongoDB",
                                                                                                    &ChaosDataService::getInstance()->setting.object_storage_setting.driver_impl);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_OBJ_STORAGE_SERVER_URL,
                                                                                                                 "The list of the obj storage servers",
                                                                                                                 &ChaosDataService::getInstance()->setting.object_storage_setting.url_list);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_OBJ_STORAGE_DRIVER_KVP,
                                                                                                                 "The key value multitoken for obj storage implementation driver (k:v)");
        
		//db
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_DB_DRIVER_IMPL,
																									"The name of the index driver implementation [MongoDB]",
																									"Mongo",
																									&ChaosDataService::getInstance()->setting.db_driver_impl);
        
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_DB_DRIVER_SERVERS,
																												 "The list of the index servers",
																												 &ChaosDataService::getInstance()->setting.db_driver_setting.servers);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_DB_DRIVER_KVP,
																												 "The key value multitoken for database implementation driver (k:v)");
		
		//initilize the faramework
		ChaosDataService::getInstance()->init(argc, argv);
		
		//
        ChaosDataService::getInstance()->start();
    } catch(chaos::CException& ex) {
      DECODE_CHAOS_EXCEPTION(ex);
      return -1;
    } catch(boost::program_options::unknown_option& ex) {
        std::cerr << ex.what() << std::endl;
	return -2;
    } catch(std::exception& ex) {
        std::cerr << ex.what() << std::endl;
	return -3;
    } catch( ... ) {
        std::cerr <<
        "Unrecognized exception, diagnostic information follows\n" <<
        boost::current_exception_diagnostic_information();
	return -4;
    }
    return 0;
}

