/*
 *	PluginLoader.cpp
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
using namespace chaos::cu::dm::driver;
using namespace chaos::common::plugin;

DriverPluginLoader::DriverPluginLoader(const char *pluginPath) : PluginLoader(pluginPath) {
    
}

DriverPluginLoader::~DriverPluginLoader() {
    driverAllocatorFunctions.clear();
}

bool DriverPluginLoader::loaded() {
    return PluginLoader::loaded();
}

PluginInspector* DriverPluginLoader::getInspectorForName(const char *pluginName) {
    return PluginLoader::getInspectorForName(pluginName);
}

AbstractDriverPlugin* DriverPluginLoader::newDriverInstance(std::string pluginName) {
    return  DriverPluginLoader::newDriverInstance(pluginName.c_str());
}

AbstractDriverPlugin* DriverPluginLoader::newDriverInstance(const char *pluginName) {
    if(!loaded()) return NULL;
    
    //check if subclass is the rigrth one
    if(!checkPluginInstantiableForSubclass(pluginName, "AbstractDriverPlugin")) return NULL;
    
    boost::function<AbstractDriverPlugin*()> driverInstancer;
    if(driverAllocatorFunctions.count(pluginName)) {
        driverInstancer = driverAllocatorFunctions[pluginName];
    } else {
        string allocatorName = string(pluginName) + SYM_ALLOC_POSTFIX;
        
        //try to get function allocator
        driverInstancer = lib.get<AbstractDriverPlugin*>(allocatorName);
        driverAllocatorFunctions.insert( make_pair<string, boost::function<AbstractDriverPlugin*()> >(pluginName, driverInstancer));
    }
    return driverInstancer();
}
