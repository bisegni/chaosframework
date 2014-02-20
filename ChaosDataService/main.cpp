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

using namespace chaos::data_service;

#define OPT_CACHE_SERVER_LIST	"cache_servers"
#define OPT_CACHE_DRIVER		"cache_driver"

int main(int argc, char * argv[]) {
    try {
		std::vector<std::string> cache_servers;
		
		//data service parameter
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_CACHE_DRIVER,
																									"Cache driver implementation",
																									"Memcached",
																									&ChaosDataService::getInstance()->settings.cache_driver_impl);
		
		ChaosDataService::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CACHE_SERVER_LIST,
																												"The list of the cache server",
																												 &ChaosDataService::getInstance()->settings.startup_chache_servers);
		
        ChaosDataService::getInstance()->init(argc, argv);
		
		if(!ChaosDataService::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CACHE_SERVER_LIST)) {
			//no cache server provided
			throw chaos::CException(-1, "No cache server provided", "Startup Sequence");
		}
		
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

