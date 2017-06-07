/*
 *	PluginManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/06/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h
#define __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h


#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/common/utility/AbstractListener.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/AbstractListenerContainer.h>

#include <boost/filesystem.hpp>

namespace chaos{
    namespace common {
        namespace plugin {
            
            typedef std::pair<std::string, std::string> ApiIdentifier;
            
            CHAOS_DEFINE_SET_FOR_TYPE(ApiIdentifier, SetApiName);
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosStringSet, MapTypeApiName);
            //! plugin information
            class LoadedPlugin {
            public:
                const boost::filesystem::path plugin_file_path;
                const uintmax_t       file_size;
                const std::time_t     last_write_time;
                LoadedPlugin(const boost::filesystem::path& _plugin_file_path);
                bool isLoaded();
                const MapTypeApiName& getRegisteredPlugin();
                bool hasPluginForClass(const std::string& plugin_class,
                                       const std::string& plugin_name);
                ChaosUniquePtr<chaos::common::plugin::PluginInspector> getInspectorForName(const std::string& api_name);
                template<typename C>
                ChaosUniquePtr<C> getInstance(const std::string& plugin_name) {
                    return ChaosUniquePtr<C>(static_cast<C*>(loader->newInstance(plugin_name)));
                }
            private:
                ChaosSharedPtr<chaos::common::plugin::PluginLoader>  loader;
                MapTypeApiName      map_type_plugin;
            };
            
            //! Comparator function
            inline bool operator==(const LoadedPlugin& lhs,
                                   const LoadedPlugin& rhs) {
                return lhs.file_size == rhs.file_size &&
                lhs.last_write_time == rhs.last_write_time;
            }
            
            //! Comparator function
            inline bool operator!=(const LoadedPlugin& lhs,
                                   const LoadedPlugin& rhs) {
                return !(lhs == rhs);
            }
            
            CHAOS_DEFINE_MAP_FOR_TYPE(boost::filesystem::path, ChaosSharedPtr<LoadedPlugin>, MapPluginLoader);
            
            //! Define base class for plugin manager listner
            class AbstractPluginManagerListner:
            public chaos::common::utility::AbstractListener {
            public:
                virtual void pluginDirectoryHasBeenUpdated() = 0;
            };
            
            //! plugin manager for execution unit api
            class PluginManager:
            public chaos::common::async_central::TimerHandler,
            public chaos::common::utility::InizializableService,
            public chaos::common::utility::Singleton<PluginManager>,
            public chaos::common::utility::AbstractListenerContainer {
                friend class chaos::common::utility::Singleton<PluginManager>;
                const boost::filesystem::path plugin_directory;
                ChaosSharedMutex map_mutex;
                MapPluginLoader map_plugin_alias_loader;
                PluginManager();
                void timeout();
                void ensurePluginDirectory();
                bool registerApi(LoadedPlugin& plugin_info);
                void fireToListener(unsigned int fire_code,
                                    chaos::common::utility::AbstractListener *listener_to_fire);
            public:
                PluginManager(const std::string& _plugin_directory);
                ~PluginManager();
                void init(void *init_data) throw(CException);
                void deinit() throw(CException);
                bool getRegisterdPluginForSubclass(const std::string& subclass,
                                                   ChaosStringVector& registered_plugin);
                template<typename C>
                ChaosUniquePtr<C> getPluginInstanceBySubclassAndName(const std::string& plugin_class,
                                                                     const std::string& plugin_name) {
                    ChaosReadLock rl(map_mutex);
                    for(MapPluginLoaderIterator it = map_plugin_alias_loader.begin(),
                        end = map_plugin_alias_loader.end();
                        it != end;
                        it++) {
                        if(it->second->hasPluginForClass(plugin_class,
                                                         plugin_name)){
                            return it->second->getInstance<C>(plugin_name);
                        }
                    }
                    return ChaosUniquePtr<C>();
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h */
