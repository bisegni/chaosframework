/*
 *	PluginManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 05/06/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/plugin/PluginManager.h>
#include <chaos/common/global.h>

#define INFO  INFO_LOG(PluginManager)
#define DBG   DBG_LOG(PluginManager)
#define ERR   ERR_LOG(PluginManager)

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace as = chaos::common::async_central;

using namespace chaos::common::plugin;
using namespace chaos::common::utility;

#pragma mark LoadedPlugin
LoadedPlugin::LoadedPlugin(const boost::filesystem::path& _plugin_file_path):
plugin_file_path(_plugin_file_path),
loader(new PluginLoader(plugin_file_path.string())),
file_size(bf::file_size(plugin_file_path)),
last_write_time(bf::last_write_time(plugin_file_path)){
    if(loader->loaded()) {
        ChaosUniquePtr<PluginDiscover> discover(loader->getDiscover());
        INFO << CHAOS_FORMAT("Register new plugin '%1%'", %plugin_file_path);
        for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
            const std::string registered_name = discover->getNameForIndex(idx);
            ChaosUniquePtr<PluginInspector> discover(loader->getInspectorForName(registered_name));
            
            const std::string api_subclass = discover->getSubclass();
            INFO << CHAOS_FORMAT("Register new api '%1%' of type '%2%'", %registered_name%api_subclass);
            map_type_plugin[api_subclass].insert(registered_name);
        }
    }
}

bool LoadedPlugin::isLoaded() {
    return loader->loaded();
}

const MapTypeApiName& LoadedPlugin::getRegisteredPlugin() {
    return map_type_plugin;
}

ChaosUniquePtr<PluginInspector> LoadedPlugin::getInspectorForName(const std::string& api_name) {
    return ChaosUniquePtr<PluginInspector>();
}

bool LoadedPlugin::hasPluginForClass(const std::string& plugin_class,
                                 const std::string& plugin_name) {
    return (map_type_plugin[plugin_class].find(plugin_name) !=  map_type_plugin[plugin_class].end());
}

#pragma PLauingManager
PluginManager::PluginManager():
plugin_directory(GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_PLUGIN_DIRECTORY_PATH)) {}

PluginManager::PluginManager(const std::string& _plugin_directory):
plugin_directory(_plugin_directory) {}

PluginManager::~PluginManager() {}

void PluginManager::init(void *init_data) throw(CException) {
    //check for plugin directory
    //ensurePluginDirectory();
    
    //plugin exists or has been created
    //as::AsyncCentralManager::getInstance()->addTimer(this, 0, 30000);
    
    //for now all plugin are loaded at sturtup
    timeout();
}

void PluginManager::deinit() throw(CException) {
    //as::AsyncCentralManager::getInstance()->removeTimer(this);
}

void PluginManager::fireToListener(unsigned int fire_code,
                                   AbstractListener *listener_to_fire) {
    AbstractPluginManagerListner *l = dynamic_cast<AbstractPluginManagerListner*>(listener_to_fire);
    if(l) l->pluginDirectoryHasBeenUpdated();
}

void PluginManager::timeout() {
    bs::error_code ec;
    bool changed = false;
    try{
        ensurePluginDirectory();
        
        // cycle through the directory
        for (bf::directory_iterator it(plugin_directory),
             end_it;
             it != end_it;
             ++it) {
            const boost::filesystem::path plugin_path = it->path();
            const boost::filesystem::path extension = it->path().extension();
            if (bf::is_regular_file(plugin_path) &&
                (extension == ".chaos_extension")) {
                ChaosSharedPtr<LoadedPlugin> plugin_info(new LoadedPlugin(plugin_path));
                if(plugin_info->isLoaded() == false) {
                    ERR << CHAOS_FORMAT("Error loading plugin '%1%'", %plugin_info->plugin_file_path);
                } else {
                    ChaosWriteLock wl(map_mutex);
                    if((changed = (map_plugin_alias_loader.count(plugin_path) == 0))) {
                        INFO << CHAOS_FORMAT("Add new plugin '%1%'", %plugin_info->plugin_file_path);
                        //add new plugin
                        map_plugin_alias_loader.insert(MapPluginLoaderPair(plugin_path, plugin_info));
                    } else if((changed = (*plugin_info != *map_plugin_alias_loader[plugin_path]))) {
                        INFO << CHAOS_FORMAT("Replace plugin '%1%' with new version", %plugin_info->plugin_file_path);
                        //plugin has changed
                        map_plugin_alias_loader[plugin_path] = plugin_info;
                    }
                }
            }
        }
        
        if(changed){fire(0);}
    } catch(CException& ex) {
        DECODE_CHAOS_EXCEPTION_ON_LOG(ERR, ex);
    } catch(...) {
        ERR << "Unknown exception";
    }
}

void PluginManager::ensurePluginDirectory() {
    if(bf::exists(plugin_directory) == false) {
        CHECK_ASSERTION_THROW_AND_LOG(bf::create_directories(plugin_directory),
                                      ERR, -1,
                                      CHAOS_FORMAT("Error creating plugin directory '%1%'", %plugin_directory));
    } else {
        CHECK_ASSERTION_THROW_AND_LOG((bf::is_directory(plugin_directory) == true),
                                      ERR, -2,
                                      CHAOS_FORMAT("Plugin directory '%1%' is not a directory", %plugin_directory));
    }
}

bool PluginManager::getRegisterdPluginForSubclass(const std::string& subclass,
                                                  ChaosStringVector& registered_api) {
    ChaosReadLock rl(map_mutex);
    for(MapPluginLoaderIterator it = map_plugin_alias_loader.begin(),
        end = map_plugin_alias_loader.end();
        it != end;
        it++) {
        const MapTypeApiName& type_plugin_name = it->second->getRegisteredPlugin();
        MapTypeApiNameConstIterator found_type = type_plugin_name.find(subclass);
        if(found_type != type_plugin_name.end()) return false;
        //add all api contained in the plugin for the specified subclass
        for(ChaosStringSetConstIterator api_it = found_type->second.begin(),
            api_end = found_type->second.end();
            api_it != api_end;
            api_it++) {
            registered_api.push_back(*api_it);
        }
    }
    return true;
}
