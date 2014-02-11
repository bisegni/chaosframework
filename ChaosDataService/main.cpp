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

#include <exception>
#include "ChaosDataService.h"

using namespace chaos::data_service;

int main(int argc, char * argv[]) {
    try {
        ChaosDataService::getInstance()->init(argc, argv);
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

