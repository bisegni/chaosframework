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

#include <chaos/common/script/lua/LuaModuleManager.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::script::lua;

namespace bf = boost::filesystem;

LuaModuleManager::LuaModuleManager() {
    const MapStrKeyStrValue& script_vm_kvp = GlobalConfiguration::getInstance()->getScriptVMKVParam();
    MapStrKeyStrValueConstIterator path_it = script_vm_kvp.find("lua-module-path");
    if(path_it != script_vm_kvp.end()) {
        //path to module path
        const bf::path lua_module_path = path_it->second;
        for (bf::directory_iterator it(lua_module_path),
             end_it;
             it != end_it;
             ++it) {
            const boost::filesystem::path plugin_path = it->path();
            const boost::filesystem::path extension = it->path().extension();
            if(extension.compare(".lua") != 0) continue;
            
            //we have a lua module to register
            map_lua_module.insert(MapLuaModulePair(plugin_path.filename().string(), plugin_path));
        }
    }
}

LuaModuleManager::~LuaModuleManager() {}

bool LuaModuleManager::hasModule(const std::string& module_name) const {
    return map_lua_module.find(module_name+".lua") != map_lua_module.end();
}

std::string LuaModuleManager::getModulePath(const std::string& module_name) const {
    MapLuaModuleConstIterator module_path = map_lua_module.find(module_name+".lua");
    if(module_path == map_lua_module.end()) return "";
    return module_path->second.string();
}
