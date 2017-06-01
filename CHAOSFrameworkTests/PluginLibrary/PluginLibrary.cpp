/*
 *	PluginLibrary.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 23/08/16 INFN, National Institute of Nuclear Physics
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

#include "PluginLibrary.h"

//define the general plugin
#include <string>

OPEN_PLUGIN_CLASS_DEFINITION(PluginAlias, 1.0, DRV, PluginLibrary)
CLOSE_PLUGIN_CLASS_DEFINITION

//define the cu driver plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DriverAlias, 1.0, ControlUnitPluginLibrary)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_address)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

OPEN_EUAPI_LUGIN_CLASS_DEFINITION(EUPluginAlgotest, 1.0, EUTestApiPLugin)
CLOSE_EUAPI_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(PluginAlias)
REGISTER_PLUGIN(DriverAlias)
REGISTER_PLUGIN(EUPluginAlgotest)
CLOSE_REGISTER_PLUGIN

PluginLibrary::PluginLibrary() {
    std::cout << "PluginTest created" << std::endl;
}

void PluginLibrary::test(int num) {
    for(int idx = 0; idx < num; idx++) {
        std::cout << "Plugin in test output" << std::endl;
    }
}


DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(ControlUnitPluginLibrary) {
    std::cout << "Driver plugin created" << std::endl;
}


int EUTestApiPLugin::execute(const char *in_data,
                             uint32_t in_data_size,
                             char **out_data,
                             uint32_t *out_data_size){
    std::string in_str(in_data, in_data_size);
    std::string ou_str = "[out]" + in_str;
    std::cout << in_str << std::endl;
    *out_data = (char*)malloc(ou_str.size());
    std::strcpy(*out_data,
                ou_str.c_str());
    *out_data_size = ou_str.size();
    return 0;
}
