/*
 *	EUPluginManager.h
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

#ifndef __CHAOSFramework__EA728CB_0258_4B88_8E67_7A49AB41A791_PluginManager_h
#define __CHAOSFramework__EA728CB_0258_4B88_8E67_7A49AB41A791_PluginManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginLoader.h>

#include <boost/filesystem.hpp>

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace script {
                namespace api {
                    namespace plugin {
                        
                        typedef std::pair<std::string, std::string> ApiIdentifier;
                        CHAOS_DEFINE_SET_FOR_TYPE(ApiIdentifier, SetApiName);
                        
                        //! plugin information
                        class EUPluginInfo {
                        public:
                            const boost::filesystem::path plugin_file_path;
                            const uintmax_t       file_size;
                            const std::time_t     last_write_time;
                            EUPluginInfo(const boost::filesystem::path& _plugin_file_path);
                            bool isLoaded();
                            const SetApiName& getApiNames();
                            ChaosUniquePtr<common::plugin::PluginInspector> getInspectorForApi(const std::string& api_name);
                            ChaosUniquePtr<EUAbstractApiPlugin> getInstanceApi(const std::string& api_name);
                        private:
                            ChaosSharedPtr<EUPluginLoader>  loader;
                            SetApiName      set_api;
                        };

                        inline bool operator==(const EUPluginInfo& lhs,
                                               const EUPluginInfo& rhs) {
                            return lhs.file_size == rhs.file_size &&
                            lhs.last_write_time == rhs.last_write_time;
                        }
                        
                        inline bool operator!=(const EUPluginInfo& lhs,
                                               const EUPluginInfo& rhs) {
                            return !(lhs == rhs);
                        }
                        
                        CHAOS_DEFINE_MAP_FOR_TYPE(boost::filesystem::path, ChaosSharedPtr<EUPluginInfo>, MapPluginLoader);
                        
                        //! plugin manager for execution unit api
                        class EUPluginManager:
                        public chaos::common::utility::InizializableService,
                        public chaos::common::async_central::TimerHandler {
                            const boost::filesystem::path plugin_directory;
                            ChaosSharedMutex map_mutex;
                            MapPluginLoader map_plugin_alias_loader;
                            void timeout();
                            void ensurePluginDirectory();
                            bool registerApi(EUPluginInfo& plugin_info);
                        public:
                            EUPluginManager(const std::string& _plugin_directory);
                            ~EUPluginManager();
                            void init(void *init_data) throw(CException);
                            void deinit() throw(CException);
                            void getRegisterdApi(SetApiName& registered_api);
                        };
                        
                    }
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__EA728CB_0258_4B88_8E67_7A49AB41A791_PluginManager_h */
