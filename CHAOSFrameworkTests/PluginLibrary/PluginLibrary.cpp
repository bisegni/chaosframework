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

OPEN_PLUGIN_CLASS_DEFINITION(PluginAlias, 1.0, DRV, PluginLibrary)
CLOSE_PLUGIN_CLASS_DEFINITION

//define the cu driver plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DriverAlias, 1.0, ControlUnitPluginLibrary)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_address)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(PluginAlias)
REGISTER_PLUGIN(DriverAlias)
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
