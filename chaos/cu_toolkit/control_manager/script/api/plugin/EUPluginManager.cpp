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

#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginManager.h>

#include <chaos/common/global.h>

#define INFO  INFO_LOG(EUPluginManager)
#define DBG   DBG_LOG(EUPluginManager)
#define ERR   ERR_LOG(EUPluginManager)

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace as = chaos::common::async_central;

using namespace chaos::common::plugin;
using namespace chaos::cu::control_manager::script::api::plugin;

#pragma mark PluginInfo
EUPluginInfo::EUPluginInfo(const boost::filesystem::path& _plugin_file_path):
plugin_file_path(_plugin_file_path),
loader(new EUPluginLoader(plugin_file_path.string())),
file_size(bf::file_size(plugin_file_path)),
last_write_time(bf::last_write_time(plugin_file_path)){
    if(loader->loaded()) {
        ChaosUniquePtr<common::plugin::PluginDiscover> discover(loader->getDiscover());
        INFO << CHAOS_FORMAT("Register new plugin '%1%'", %plugin_file_path);
        for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
            const std::string registered_name = discover->getNameForIndex(idx);
            INFO << CHAOS_FORMAT("Register new api '%1%'", %registered_name);
            set_api.insert(ApiIdentifier(plugin_file_path.filename().string(),
                                         registered_name));
        }
    }
}

bool EUPluginInfo::isLoaded() {
    return loader->loaded();
}

const SetApiName& EUPluginInfo::getApiNames() {
    return set_api;
}

ChaosUniquePtr<PluginInspector> getInspectorForApi(const std::string& api_name) {
    return ChaosUniquePtr<PluginInspector>();
}

ChaosUniquePtr<EUAbstractApiPlugin> EUPluginInfo::getInstanceApi(const std::string& api_name) {
    return ChaosUniquePtr<EUAbstractApiPlugin>();
}

#pragma EUPLauingManager
EUPluginManager::EUPluginManager(const std::string& _plugin_directory):
plugin_directory(_plugin_directory) {}

EUPluginManager::~EUPluginManager() {}

void EUPluginManager::init(void *init_data) throw(CException) {
    //check for plugin directory
    ensurePluginDirectory();
    
    //plugin exists or has been created
    as::AsyncCentralManager::getInstance()->addTimer(this, 0, 30000);
}

void EUPluginManager::deinit() throw(CException) {
    as::AsyncCentralManager::getInstance()->removeTimer(this);
}

void EUPluginManager::timeout() {
    bs::error_code ec;
    try{
        ensurePluginDirectory();
        
        // cycle through the directory
        for (bf::directory_iterator it(plugin_directory),
             end_it;
             it != end_it;
             ++it) {
            const boost::filesystem::path plugin_path = it->path();
            if (bf::is_regular_file(plugin_path)) {
                ChaosSharedPtr<EUPluginInfo> plugin_info(new EUPluginInfo(plugin_path));
                if(plugin_info->isLoaded() == false) {
                    ERR << CHAOS_FORMAT("Error loading plugin '%1%'", %plugin_info->plugin_file_path);
                } else {
                    ChaosWriteLock wl(map_mutex);
                    if(map_plugin_alias_loader.count(plugin_path) == 0) {
                        //add new plugin
                        map_plugin_alias_loader.insert(MapPluginLoaderPair(plugin_path, plugin_info));
                    } else if(*plugin_info != *map_plugin_alias_loader[plugin_path]){
                        //plugin has changed
                        map_plugin_alias_loader[plugin_path] = plugin_info;
                    }
                }
            }
        }
    } catch(CException& ex) {
        DECODE_CHAOS_EXCEPTION_ON_LOG(ERR, ex);
    } catch(...) {
        ERR << "Unknown exception";
    }
}

void EUPluginManager::ensurePluginDirectory() {
    if(bf::exists(plugin_directory) == false) {
        CHECK_ASSERTION_THROW_AND_LOG(bf::create_directories(plugin_directory),
                                      ERR, -1,
                                      CHAOS_FORMAT("Error creating plugin directory '%1%'", %plugin_directory));
    } else {
        CHECK_ASSERTION_THROW_AND_LOG((bf::is_directory(plugin_directory) == false),
                                      ERR, -2,
                                      CHAOS_FORMAT("Plugin directory '%1%' is not a directory", %plugin_directory));
    }
}

void EUPluginManager::getRegisterdApi(SetApiName& registered_api) {
    ChaosReadLock rl(map_mutex);
    for(MapPluginLoaderIterator it = map_plugin_alias_loader.begin(),
        end = map_plugin_alias_loader.end();
        it != end;
        it++) {
        const SetApiName& plugin_api_names = it->second->getApiNames();
        registered_api.insert(plugin_api_names.begin(), plugin_api_names.end());
    }
}
