/*
 *	LuaModule.cpp
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

#include <chaos/common/script/lua/LuaModuleManager.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::script::lua;

namespace bf = boost::filesystem;

LuaModuleManager::LuaModuleManager():
lua_module_path(GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_PLUGIN_DIRECTORY_PATH)) {
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

LuaModuleManager::~LuaModuleManager() {}

bool LuaModuleManager::hasModule(const std::string& module_name) const {
    return map_lua_module.find(module_name) != map_lua_module.end();
}

std::string LuaModuleManager::getModulePath(const std::string& module_name) const {
    MapLuaModuleConstIterator module_path = map_lua_module.find(module_name);
    if(module_path == map_lua_module.end()) return "";
    return module_path->second.string();
}
