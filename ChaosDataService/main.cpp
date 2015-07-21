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
																									"Memcached",
																									&ChaosDataService::getInstance()->setting.cache_driver_setting.cache_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_SERVER_LIST,
																												 "The list of the cache server",
																												 &ChaosDataService::getInstance()->setting.cache_driver_setting.startup_chache_servers);
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CACHE_WORKER_NUM,
																									 "The number of the cache worker",
																									 CACHE_WORKER_NUMBER,
																									 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CACHE_WORKER_THREAD,
																								 "The thread number of each cache worker",
																								 1,
																								 &ChaosDataService::getInstance()->setting.cache_driver_setting.caching_worker_setting.job_thread_number);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP,
																												 "The key value parameter for cache implementation driver (ex k:v-k1:v1)");
		
		//query consumer
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_QUERY_CONSUMER_VFILE_MANTAINANCE_DELAY,
																								 "Repeat time for virtual file mantainer in query consumer in seconds",
																								 QUERY_CONSUMER_VFILE_MANTAINANCE_DEFAULT_DELAY,
																								 &ChaosDataService::getInstance()->setting.vfile_mantainer_delay);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_QUERY_CONSUMER_QM_THREAD_POOL_SIZE,
																									 "Number ofthread in the pool of query consumer",
																									 QUERY_CONSUMER_QM_THREAD_POOL_SIZE,
																									 &ChaosDataService::getInstance()->setting.query_manager_thread_poll_size);
		//indexer
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_INDEXER_WORKER_NUM,
																								 "The number of the indexer worker",
																								 INDEXER_DEFAULT_WORKER_NUMBER,
																								 &ChaosDataService::getInstance()->setting.indexer_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_INDEXER_SCAN_DELAY,
																								 "The repeat delay for virtual system scan in seconds",
																								 INDEXER_DEFAULT_SCAN_DELAY,
																								 &ChaosDataService::getInstance()->setting.indexer_scan_delay);
		
		//vfs conf
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_STORAGE_DRIVER_IMPL,
																									"The name of the vfs storage implementation [Posix]",
																									"Posix",
																									&ChaosDataService::getInstance()->setting.file_manager_setting.storage_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_STORAGE_DOMAIN,
																									"The name of the domain exposed by the driver",
																									"CHAOS_DOMAIN",
																									&ChaosDataService::getInstance()->setting.file_manager_setting.storage_driver_setting.domain.name);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_VFS_STORAGE_DRIVER_KVP,
																												 "The key value parameter for storage implementation driver (ex k:v-k1:v1)");
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_VFS_STORAGE_MAX_BLOCK_LIFETIME,
																								 "Is the lifetime that every bloc is valid to accept data",
																								 VFSManager_MAX_BLOCK_LIFETIME,
																								 &ChaosDataService::getInstance()->setting.file_manager_setting.max_block_lifetime);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_VFS_STORAGE_MAX_BLOCK_SIZE,
																								 "Is the max size that a block can reach",
																								 VFSManager_MAX_BLOCK_SIZE,
																								 &ChaosDataService::getInstance()->setting.file_manager_setting.max_block_size);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_VFS_STORAGE_LOG_METRIC,
                                                                                             "Enable log metric on file system driver",
                                                                                             false,
                                                                                             &ChaosDataService::getInstance()->setting.file_manager_setting.storage_driver_setting.log_metric);
        
        ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< uint32_t >(OPT_VFS_STORAGE_LOG_METRIC_UPDATE_INTERVAL,
                                                                                                 "Specify the file update interval, in second, for the metric of the system driver",
                                                                                                 1,
                                                                                                 &ChaosDataService::getInstance()->setting.file_manager_setting.storage_driver_setting.log_metric_update_interval);
        
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

