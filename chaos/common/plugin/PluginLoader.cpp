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

#include <cstring>
#include <chaos/common/plugin/PluginLoader.h>


using namespace std;
using namespace boost::extensions;
using namespace chaos::common::plugin;

PluginLoader::PluginLoader(const char *pluginPath):lib(pluginPath) {
    if(pluginPath) {
        //find library
        if (lib.open()) {
            // load the symbol for retrieve the plugin inspector
            getDiscoverFunction = lib.get<PluginDiscover*>("getDiscover");
        }
    }
}

PluginLoader::~PluginLoader() {
}

bool PluginLoader::checkPluginInstantiableForSubclass(const char * pluginName, const char * subclass) {
    if(!loaded()) return false;
    //check inspector if we can instanziate the plugin
    auto_ptr<PluginInspector> inspector(getInspectorForName(pluginName));

    //check if the inspector was defined
    if(!inspector.get()) return false;

    //check the plugin subclass
    if(strcmp(inspector->getSubclass(), subclass) != 0) return false;
    
    return true;
}

bool PluginLoader::loaded() {
    return lib.is_open() && getDiscoverFunction != NULL;
}

PluginDiscover* PluginLoader::getDiscover() {
    return getDiscoverFunction();
}

PluginInspector* PluginLoader::getInspectorForName(const char *pluginName) {
    boost::function<PluginInspector*()> getInspectorFunction;
    string inspectorName = string(pluginName) + PLUGIN_INSPECTOR_POSTFIX;
    getInspectorFunction = lib.get<PluginInspector*>(inspectorName);
    if(getInspectorFunction == NULL) return NULL;
    return getInspectorFunction();
}

AbstractPlugin* PluginLoader::newInstance(std::string pluginName) {
    return newInstance(pluginName.c_str());
}

AbstractPlugin* PluginLoader::newInstance(const char *pluginName) {
    
    //check if lib is loaded
    if(!loaded()) return NULL;
    
    //check if subclass is the rigrth one
    if(!checkPluginInstantiableForSubclass(pluginName, "AbstractPlugin")) return NULL;
    
    //we can instantiate the plugin
    string allocatorName = string(pluginName) + SYM_ALLOC_POSTFIX;
        
    //try to get function allocator
    boost::function<AbstractPlugin*()>  instancer = lib.get<AbstractPlugin*>(allocatorName);
    return instancer != NULL ? instancer():NULL;
}
