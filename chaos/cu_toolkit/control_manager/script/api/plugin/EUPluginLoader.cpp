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

#include <chaos/common/chaos_types.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginLoader.h>

using namespace std;
using namespace boost::extensions;
using namespace chaos::common::plugin;
using namespace chaos::cu::control_manager::script::api::plugin;

//! default constructor
EUPluginLoader::EUPluginLoader(const std::string& plugin_path):
PluginLoader(plugin_path) {}

//! default desctructor
EUPluginLoader::~EUPluginLoader() {
    driverAllocatorFunctions.clear();
}

//! give infromation about the load operation of the dll
bool EUPluginLoader::loaded() {
    return PluginLoader::loaded();
}

ChaosUniquePtr<PluginDiscover> EUPluginLoader::getDiscover() {
    return ChaosUniquePtr<PluginDiscover>(PluginLoader::getDiscover());
}

//! Return the instance of the driver
ChaosUniquePtr<PluginInspector> EUPluginLoader::getInspectorForName(const std::string& plugin_name) {
    return ChaosUniquePtr<PluginInspector>(PluginLoader::getInspectorForName(plugin_name.c_str()));
}

//! Get the inspector for the name
ChaosUniquePtr<EUAbstractApiPlugin> EUPluginLoader::newPluginInstance(const std::string& plugin_name) {
    if(!loaded()) return ChaosUniquePtr<EUAbstractApiPlugin>();
    
    //check if subclass is the rigrth one
    if(!checkPluginInstantiableForSubclass(plugin_name.c_str(), "chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin")) return ChaosUniquePtr<EUAbstractApiPlugin>();
    
    std::string allocator_name = plugin_name + SYM_ALLOC_POSTFIX;
    //try to get function allocator
    boost::function<EUAbstractApiPlugin*()> driverInstancer = lib.get<EUAbstractApiPlugin*>(allocator_name);
    
    //return instanc if we have found the instancer symbol
    return ChaosUniquePtr<EUAbstractApiPlugin>((driverInstancer != NULL)?driverInstancer():NULL);
}
