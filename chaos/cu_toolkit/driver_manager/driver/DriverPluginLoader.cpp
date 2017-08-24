/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
    if(!checkPluginInstantiableForSubclass(pluginName, "chaos::cu::driver_manager::driver::AbstractDriverPlugin")) return NULL;

    string allocatorName = string(pluginName) + SYM_ALLOC_POSTFIX;
    //try to get function allocator
    boost::function<AbstractDriverPlugin*()> driverInstancer = lib.get<AbstractDriverPlugin*>(allocatorName);
    
    //return instanc if we have found the instancer symbol
    return (driverInstancer != NULL)?driverInstancer():NULL;
}
