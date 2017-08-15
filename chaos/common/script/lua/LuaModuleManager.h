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
