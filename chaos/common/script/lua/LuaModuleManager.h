/*
 *	LuaModule.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/06/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EA042F22_DC3E_44C4_BEDE_8BA9FE7FB4DF_LuaModule_h
#define __CHAOSFramework_EA042F22_DC3E_44C4_BEDE_8BA9FE7FB4DF_LuaModule_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>

#include <boost/filesystem.hpp>

namespace chaos {
    namespace common {
        namespace script {
            namespace lua {
                
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::filesystem::path, MapLuaModule);
                
                class LuaModuleManager:
                public chaos::common::utility::Singleton<LuaModuleManager> {
                    friend class chaos::common::utility::Singleton<LuaModuleManager>;
                    //!contiane module name and his path
                    MapLuaModule map_lua_module;
                    LuaModuleManager();
                    ~LuaModuleManager();
                public:
                    bool hasModule(const std::string& module_name) const;
                    
                    std::string getModulePath(const std::string& module_name) const;
                };

            }
        }
    }
}

#endif /* __CHAOSFramework_EA042F22_DC3E_44C4_BEDE_8BA9FE7FB4DF_LuaModule_h */
