/*
 *	DriverPLuginLoader.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/driver_manager/driver/DriverPluginLoader.h>


using namespace std;
using namespace boost::extensions;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::common::plugin;

//! default constructor
DriverPluginLoader::DriverPluginLoader(const char *pluginPath) : PluginLoader(pluginPath) {}

//! default desctructor
DriverPluginLoader::~DriverPluginLoader() {
    driverAllocatorFunctions.clear();
}

//! give infromation about the load operation of the dll
bool DriverPluginLoader::loaded() {
    return PluginLoader::loaded();
}

//! Return the instance of the driver
PluginInspector* DriverPluginLoader::getInspectorForName(const char *pluginName) {
    return PluginLoader::getInspectorForName(pluginName);
}

//! Get the inspector for the name
AbstractDriverPlugin* DriverPluginLoader::newDriverInstance(std::string pluginName) {
    return  DriverPluginLoader::newDriverInstance(pluginName.c_str());
}
//! Return the instance of the driver
AbstractDriverPlugin* DriverPluginLoader::newDriverInstance(const char *pluginName) {
    if(!loaded()) return NULL;
    
    //check if subclass is the rigrth one
    if(!checkPluginInstantiableForSubclass(pluginName, "AbstractDriverPlugin")) return NULL;

    string allocatorName = string(pluginName) + SYM_ALLOC_POSTFIX;
    //try to get function allocator
    boost::function<AbstractDriverPlugin*()> driverInstancer = lib.get<AbstractDriverPlugin*>(allocatorName);
    
    //return instanc if we have found the instancer symbol
    return (driverInstancer != NULL)?driverInstancer():NULL;
}
