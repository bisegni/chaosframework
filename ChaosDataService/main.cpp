/*
 *	main.cpp
 *	!CHOAS
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

#include "ChaosDataService.h"

#include <vector>
#include <exception>
#include <fstream>      // std::ifstream

using namespace chaos::data_service;


int main(int argc, char * argv[]) {
    try {
		std::vector<std::string> cache_servers;
		
		//cache parameter
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_CONF_FILE,
																									"File configuration path");
		
		//cache parameter
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_CACHE_DRIVER,
																									"Cache driver implementation",
																									"Memcached",
																									&ChaosDataService::getInstance()->settings.cache_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_SERVER_LIST,
																												"The list of the cache server",
																												 &ChaosDataService::getInstance()->settings.startup_chache_servers);
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CACHE_WORKER_NUM,
																									 "The number of the cache worker",
																									 CACHE_WORKER_NUMBER,
																									 &ChaosDataService::getInstance()->settings.caching_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_CACHE_WORKER_THREAD,
																									"The thread number of each cache worker",
																									1,
																									 &ChaosDataService::getInstance()->settings.caching_worker_setting.job_thread_number);
		
		//answer conf
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_ANSWER_WORKER_NUM,
																									 "The number of the answer worker",
																									 ANSWER_WORKER_NUMBER,
																									 &ChaosDataService::getInstance()->settings.answer_worker_num);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< unsigned int >(OPT_ANSWER_WORKER_THREAD,
																									 "The thread number of each answer worker",
																									 1,
																									 &ChaosDataService::getInstance()->settings.answer_worker_setting.job_thread_number);

		//vfs conf
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_STORAGE_DRIVER_IMPL,
																									"The name of the vfs storage implementation [Posix]",
																									"Posix",
																									&ChaosDataService::getInstance()->settings.file_manager_setting.storage_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_STORAGE_DOMAIN,
																									"The name of the domain exposed by the driver",
																									"CHAOS_DOMAIN",
																									&ChaosDataService::getInstance()->settings.file_manager_setting.storage_driver_setting.domain.name);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_STORAGE_DRIVER_KVP,
																									"The key value parameter for storage implementation driver (ex k:v-k1:v1)");
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_INDEX_DRIVER_IMPL,
																									"The name of the vfs storage implementation [MongoDB]",
																									"MongoDB",
																									&ChaosDataService::getInstance()->settings.file_manager_setting.index_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_VFS_INDEX_DRIVER_SERVERS,
																												 "The list of the index servers",
																												 &ChaosDataService::getInstance()->settings.file_manager_setting.index_driver_setting.servers);
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_VFS_INDEX_DRIVER_KVP,
																									"The key value parameter for index implementation driver (ex k:v-k1:v1)");

		//preparse for blow custom option
		ChaosDataService::getInstance()->preparseCommandOption(argc, argv);
	
		//check if we have a config file
		if(ChaosDataService::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CONF_FILE)) {
			//reload configuraiton from file
			std::string file_option = ChaosDataService::getInstance()->getGlobalConfigurationInstance()->getOption<std::string>(OPT_CONF_FILE);
			std::ifstream option_file_stream;
			option_file_stream.open(file_option.c_str(), std::ifstream::in);
			if(!option_file_stream) {
				throw chaos::CException(-1, "Error opening configuration file", "Startup sequence");
			}
			//reparse the config file
			ChaosDataService::getInstance()->preparseConfigFile(option_file_stream);
		}
		//parse the dafult framework option
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->checkDefaultOption();
		
		//initilize the faramework
		ChaosDataService::getInstance()->init(NULL);
		
        ChaosDataService::getInstance()->start();
    } catch(chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    } catch(boost::program_options::unknown_option& ex) {
        std::cerr << ex.what() << std::endl;
    } catch(std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    } catch( ... ) {
        std::cerr <<
        "Unrecognized exception, diagnostic information follows\n" <<
        boost::current_exception_diagnostic_information();
    }
    return 0;
}

