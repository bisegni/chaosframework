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

#include <cstring>
#include <chaos/common/chaos_types.h>
#include <chaos/common/plugin/PluginLoader.h>


using namespace std;
using namespace boost::extensions;
using namespace chaos::common::plugin;

PluginLoader::PluginLoader(const std::string& plugin_path):
lib(plugin_path)  {
    //find library
    if (lib.open()) {
        // load the symbol for retrieve the plugin inspector
        getDiscoverFunction = lib.get<PluginDiscover*>("getDiscover");
    }
}

PluginLoader::~PluginLoader() {
}

bool PluginLoader::checkPluginInstantiableForSubclass(const std::string& pluginName,
                                                      const std::string& subclass) {
    if(!loaded()) return false;
    //check inspector if we can instanziate the plugin
    ChaosUniquePtr<PluginInspector> inspector(getInspectorForName(pluginName));
    
    //check if the inspector was defined
    if(!inspector.get()) return false;
    
    //check the plugin subclass
    if(subclass.compare(inspector->getSubclass()) != 0) return false;
    
    return true;
}

bool PluginLoader::loaded() {
    return lib.is_open() && getDiscoverFunction != NULL;
}

PluginDiscover* PluginLoader::getDiscover() {
    return getDiscoverFunction();
}

PluginInspector* PluginLoader::getInspectorForName(const std::string& pluginName) {
    boost::function<PluginInspector*()> getInspectorFunction;
    string inspectorName = string(pluginName) + PLUGIN_INSPECTOR_POSTFIX;
    getInspectorFunction = lib.get<PluginInspector*>(inspectorName);
    if(getInspectorFunction == NULL) return NULL;
    return getInspectorFunction();
}
