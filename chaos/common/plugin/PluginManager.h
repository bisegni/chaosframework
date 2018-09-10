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

#ifndef __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h
#define __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/common/utility/AbstractListener.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/AbstractListenerContainer.h>
#include <chaos/common/utility/LockableObject.h>

#include <boost/filesystem.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace chaos{
    namespace common {
        namespace plugin {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosStringSet, MapTypeApiName);
            //! plugin information
            class LoadedPluginLib {
            public:
                const boost::filesystem::path plugin_file_path;
                const uintmax_t       file_size;
                const std::time_t     last_write_time;
                LoadedPluginLib(const boost::filesystem::path& _plugin_file_path);
                bool isLoaded();
                const MapTypeApiName& getRegisteredPlugin();
                bool hasPluginForClass(const std::string& plugin_class,
                                       const std::string& plugin_name);
                ChaosUniquePtr<chaos::common::plugin::PluginInspector> getInspectorForName(const std::string& api_name);
                template<typename C>
                ChaosUniquePtr<C> getInstance(const std::string& plugin_name) {
                    return loader->newInstance<C>(plugin_name);
                }
            private:
                ChaosSharedPtr<chaos::common::plugin::PluginLoader>  loader;
                MapTypeApiName      map_type_plugin;
            };
            
            //! Comparator function
            inline bool operator==(const LoadedPluginLib& lhs,
                                   const LoadedPluginLib& rhs) {
                return lhs.file_size == rhs.file_size &&
                lhs.last_write_time == rhs.last_write_time;
            }
            
            //! Comparator function
            inline bool operator!=(const LoadedPluginLib& lhs,
                                   const LoadedPluginLib& rhs) {
                return !(lhs == rhs);
            }
            
            typedef std::pair<std::string, std::string> PluginIdentifier;
            CHAOS_DEFINE_MAP_FOR_TYPE(PluginIdentifier, boost::filesystem::path, MapPluginIdentifierPath);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapPluginIdentifierPath, LockableMPIdPath);
            
            CHAOS_DEFINE_MAP_FOR_TYPE(boost::filesystem::path, ChaosSharedPtr<LoadedPluginLib>, MapPluginLoader);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapPluginLoader, LockableMPLoader);
            
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
                
                LockableMPIdPath map_pidentifier_path;
                LockableMPLoader map_lib_path_loader;
                PluginManager();
                void timeout();
                void ensurePluginDirectory();
                void updatePluginDatabase(LoadedPluginLib& loaded_lib);
                void fireToListener(unsigned int fire_code,
                                    chaos::common::utility::AbstractListener *listener_to_fire);
            public:
                PluginManager(const std::string& _plugin_directory);
                ~PluginManager();
                void init(void *init_data);
                void deinit();
                bool getRegisterdPluginForSubclass(const std::string& plugin_subclass,
                                                   ChaosStringVector& registered_plugin);
                template<typename C>
                ChaosUniquePtr<C> getPluginInstanceBySubclassAndName(const std::string& plugin_class,
                                                                     const std::string& plugin_name) {
                    LockableMPIdPathReadLock rl_1 = map_pidentifier_path.getReadLockObject();
                    LockableMPLoaderReadLock rl_2 = map_lib_path_loader.getReadLockObject();
                    PluginIdentifier identifier_key(plugin_class, plugin_name);
                    MapPluginIdentifierPathIterator path_it = map_pidentifier_path().find(identifier_key);
                    if(path_it == map_pidentifier_path().end()) return ChaosUniquePtr<C>();
                    return map_lib_path_loader()[path_it->second]->getInstance<C>(plugin_name);
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_A6357109_5878_4143_9231_88AE4DD31250_PluginManager_h */
