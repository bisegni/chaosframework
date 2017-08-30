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

#include <chaos/common/global.h>
#include <chaos/common/script/lua/LuaScriptVM.h>
#include <chaos/common/script/lua/LuaModuleManager.h>
//include lua modules
#include <chaos/common/script/lua/lib/json.h>
#include <chaos/common/script/lua/lib/base64.h>

#include <string>

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::script::lua;

DEFINE_CLASS_FACTORY(LuaScriptVM, AbstractScriptVM);

#define LSVM_INFO   INFO_LOG(LuaScriptVM)
#define LSVM_IDBG   INFO_LOG(LuaScriptVM)
#define LSVM_ERR    ERR_LOG(LuaScriptVM)

const char ChaosLuaWrapperInterface::className[] = "chaos";
const Luna<ChaosLuaWrapperInterface>::RegType ChaosLuaWrapperInterface::Register[] = {
    { "callApi", &ChaosLuaWrapperInterface::callApi },
    { 0 }
};

ChaosLuaWrapperInterface::ChaosLuaWrapperInterface(lua_State *L):
script_caller(NULL){
    LSVM_IDBG << "ChaosLuaWrapperInterface constructor\n";
}

ChaosLuaWrapperInterface::~ChaosLuaWrapperInterface() {
    LSVM_IDBG << "ChaosLuaWrapperInterface destructor\n";
}

int ChaosLuaWrapperInterface::callApi(lua_State *ls) {
    int err = 0;
    //the minimum number of parameter are two (class and api name)
    int nargs = lua_gettop(ls);
    if(nargs < 3) {
        //return error
        lua_pushinteger(ls, -1);
        return 1;
    }
    
    ScriptInParam   in_param;
    ScriptOutParam  out_param;
    //get api class and function names
    const std::string class_api_name = lua_tostring(ls, 2);
    const std::string api_name = lua_tostring(ls, 3);
    
    //scan all input parameter
    for (int i=4; i <= nargs; i++) {
        switch(lua_type(ls, i)) {
            case LUA_TSTRING:
                in_param.push_back(CDataVariant(std::string(lua_tostring(ls, i))));
                break;
            case LUA_TNUMBER:
                if(lua_isinteger(ls, i)){
                    in_param.push_back(CDataVariant((int64_t)lua_tointeger(ls, i)));
                } else if (lua_isnumber(ls, i)) {
                    in_param.push_back(CDataVariant(lua_tonumber(ls, i)));
                }
                break;
            case LUA_TBOOLEAN:
                in_param.push_back(CDataVariant(lua_toboolean(ls, i)));
                break;
            default:
                break;
        }
    }
    
    lua_getglobal(ls, "script_vm");
    LuaScriptVM *lua_vm = static_cast<LuaScriptVM*>(lua_touserdata(ls, -1));
    
    if((err = lua_vm->getCaller()->callScriptApi(class_api_name,
                                                 api_name,
                                                 in_param,
                                                 out_param))) {
        LSVM_ERR << CHAOS_FORMAT("Error %1% calling %2%[%3%]", %err%api_name%class_api_name);
        luaL_argerror(ls, err, CHAOS_FORMAT("Error executing %1%[%2%]", %api_name%class_api_name).c_str());
    } else {
        //we can send the return
        for(ScriptOutParamIterator it = out_param.begin(),
            end = out_param.end();
            it != end;
            it++) {
            switch(it->getType()) {
                case DataType::TYPE_BOOLEAN:
                    lua_pushboolean(ls, it->asBool());
                    break;
                case DataType::TYPE_INT32:
                    lua_pushinteger(ls, it->asInt32());
                    break;
                case DataType::TYPE_INT64:
                    lua_pushinteger(ls, it->asInt64());
                    break;
                case DataType::TYPE_DOUBLE:
                    lua_pushnumber(ls, it->asDouble());
                    break;
                case DataType::TYPE_CLUSTER:
                case DataType::TYPE_STRING:
                    lua_pushstring(ls, it->asString().c_str());
                    break;
                case DataType::TYPE_BYTEARRAY:{
                    const CDataBuffer *buf = it->asCDataBuffer();
                    lua_pushlstring(ls, buf->getBuffer(), buf->getBufferSize());
                    break;
                }
                default:
                    break;
            }
        }
    }
    return (int)out_param.size();
}

static int lua_print_wrap(lua_State* ls) {
    int nargs = lua_gettop(ls);
    std::stringstream ss;
    for (int i=1; i <= nargs; i++) {
        switch(lua_type(ls, 1)) {
            case LUA_TSTRING:
                ss << lua_tostring(ls, i);
                break;
            case LUA_TNUMBER:
                if(lua_isinteger(ls, i)){
                    ss << lua_tointeger(ls, i);
                } else if (lua_isnumber(ls, i)) {
                    ss << lua_tonumber(ls, i);
                }
                break;
            default:
                break;
        }
    }
    LSVM_INFO << ss.str();
    return 0;
}

static int lua_loader(lua_State* ls) {
    int err = 0;
    // get the module name
    const std::string required_package = lua_tostring(ls, 1);
    // find if you have such module loaded
    if (required_package.compare("json") == 0) {
        if((err = luaL_loadbuffer(ls, (const char *)json_lua, (size_t)json_lua_len, required_package.c_str()))) {
            LSVM_ERR << lua_tostring(ls, -1);
            lua_pop(ls, 1);
        }
    } else if (required_package.compare("base64") == 0) {
        if((err = luaL_loadbuffer(ls, (const char *)base64_lua, (size_t)base64_lua_len, required_package.c_str()))) {
            LSVM_ERR << lua_tostring(ls, -1);
            lua_pop(ls, 1);
        }
    } else  if(LuaModuleManager::getInstance()->hasModule(required_package)) {
        const std::string lua_module_path = LuaModuleManager::getInstance()->getModulePath(required_package);
        LSVM_INFO << CHAOS_FORMAT("Loading module %1% from file %2%", %required_package%lua_module_path);
        if((err = luaL_loadfile(ls, lua_module_path.c_str()))) {
            LSVM_ERR << lua_tostring(ls, -1);
            lua_pop(ls, 1);
        }
    }
    return 1;
}

static const struct luaL_Reg chaos_wrap [] = {
    {"_G", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_UTF8LIBNAME, luaopen_utf8},
    {NULL, NULL} /* end of array */
};

static const struct luaL_Reg chaos_custom [] = {
    {"print", lua_print_wrap},
    //{"require", lua_require_wrap},
    {NULL, NULL} /* end of array */
};

LuaScriptVM::LuaScriptVM(const std::string& name):
AbstractScriptVM(name),
ls(NULL){}

LuaScriptVM::~LuaScriptVM() {
    if(ls){lua_close(ls);}
}

void LuaScriptVM::init(void *init_data) throw(chaos::CException) {
    ls = luaL_newstate();
    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = chaos_wrap; lib->func; lib++) {
        luaL_requiref(ls, lib->name, lib->func, 1);
        lua_pop(ls, 1);  /* remove lib */
    }
    //luaL_openlibs(ls);
    lua_getglobal(ls, "_G");
    luaL_setfuncs(ls, chaos_custom, 0);
    lua_pop(ls, 1);
    
    //set custom loader
    lua_register(ls, "chaos_module_loader", lua_loader);
    luaL_dostring(ls, "table.insert(package.searchers, 2, chaos_module_loader) \n");
    
    //register wrapper interface
    Luna<ChaosLuaWrapperInterface>::Register(ls);
    
}

void LuaScriptVM::deinit() throw(chaos::CException) {
    if(ls){
        lua_close(ls);
        ls = NULL;
    }
}

void LuaScriptVM::allocationOf(ChaosLuaWrapperInterface *newAllocatedClass) {
    //set the script caller with helper interface
    newAllocatedClass->script_caller = script_caller;
}

void LuaScriptVM::deallocationOf(ChaosLuaWrapperInterface *deallocatedClass) {
    
}

int LuaScriptVM::loadScript(const std::string& loadable_script) {
    int err = 0;
    
    lua_pushlightuserdata(ls, static_cast<void*>(this));
    lua_setglobal(ls, "script_vm");
    
    //load script
    if((err = luaL_loadstring(ls, loadable_script.c_str()))){
        LSVM_ERR << CHAOS_FORMAT("Error %1% loading script", %lua_tostring(ls, -1));
    } else if((err = (lua_pcall(ls, 0, 0, 0)))){
        LSVM_ERR << CHAOS_FORMAT("Error %1% compiling script", %lua_tostring(ls, -1));
    }
    return err;
}

int LuaScriptVM::callFunction(const std::string& function_name,
                              const ScriptInParam& input_parameter,
                              ScriptOutParam& output_parameter) {
    last_error = 0;
    last_error_message.clear();
    
    // push functions and arguments
    lua_getglobal(ls, function_name.c_str());
    
    for(ScriptInParamConstIterator it = input_parameter.begin(),
        end = input_parameter.end();
        it != end;
        it++) {
        switch(it->getType()) {
            case DataType::TYPE_BOOLEAN:
                lua_pushboolean(ls, it->asBool());
                break;
            case DataType::TYPE_INT32:
                lua_pushinteger(ls, it->asInt32());
                break;
            case DataType::TYPE_INT64:
                lua_pushinteger(ls, it->asInt64());
                break;
            case DataType::TYPE_DOUBLE:
                lua_pushnumber(ls, it->asDouble());
                break;
                
            case DataType::TYPE_CLUSTER:
            case DataType::TYPE_STRING:
                lua_pushstring(ls, it->asString().c_str());
                break;
            case DataType::TYPE_BYTEARRAY:{
                const CDataBuffer *buf = it->asCDataBuffer();
                lua_pushlstring(ls, buf->getBuffer(), buf->getBufferSize());
                break;
            }
            default:
                break;
        }
    }
    
    // do the call (2 arguments, 1 result)
    if ((last_error = lua_pcall(ls,
                                (int)input_parameter.size(),
                                LUA_MULTRET,
                                1) != 0)) {
        LSVM_ERR << CHAOS_FORMAT("Error %1% calling script function %2%", %(last_error_message = lua_tostring(ls, -1))%function_name);
    } else {
        //result_element = lua_gettop(ls);
        //execution as gone well, we can get the expected result
        //        for(int idx = 0;
        //            idx < result_element;
        //            idx++) {
        int type = 0;
        switch((type = lua_type(ls, -1))){
            case LUA_TSTRING:
                output_parameter.push_back(CDataVariant(lua_tostring(ls, -1)));
                break;
            case LUA_TNUMBER:
                if(lua_isinteger(ls, -1)){
                    output_parameter.push_back(CDataVariant((int64_t)lua_tointeger(ls, -1)));
                } else if (lua_isnumber(ls, -1)) {
                    output_parameter.push_back(CDataVariant(lua_isnumber(ls, -1)));
                }
                break;
            case LUA_TBOOLEAN:
                output_parameter.push_back(CDataVariant(lua_toboolean(ls, -1)));
                break;
            default:
                break;
        }
        lua_pop(ls, 1);
        //        }
    }
    return last_error;
}

int LuaScriptVM::callProcedure(const std::string& function_name,
                               const ScriptInParam& input_parameter) {
    last_error = 0;
    last_error_message.clear();
    
    // push functions and arguments
    lua_getglobal(ls, function_name.c_str());
    
    for(ScriptInParamConstIterator it = input_parameter.begin(),
        end = input_parameter.end();
        it != end;
        it++) {
        switch(it->getType()) {
            case DataType::TYPE_BOOLEAN:
                lua_pushboolean(ls, it->asBool());
                break;
            case DataType::TYPE_INT32:
                lua_pushinteger(ls, it->asInt32());
                break;
            case DataType::TYPE_INT64:
                lua_pushinteger(ls, it->asInt64());
                break;
            case DataType::TYPE_DOUBLE:
                lua_pushnumber(ls, it->asDouble());
                break;
            case DataType::TYPE_CLUSTER:
            case DataType::TYPE_STRING:
                lua_pushstring(ls, it->asString().c_str());
                break;
            case DataType::TYPE_BYTEARRAY:{
                const CDataBuffer *buf = it->asCDataBuffer();
                lua_pushlstring(ls, buf->getBuffer(), buf->getBufferSize());
                break;
            }
            default:
                break;
        }
    }
    
    // do the call (2 arguments, 1 result)
    if ((last_error = lua_pcall(ls,
                                (int)input_parameter.size(),
                                0,
                                0) != 0)) {
        LSVM_ERR << CHAOS_FORMAT("Error %1% calling script function %2%", %(last_error_message = lua_tostring(ls, -1))%function_name);
    }
    return last_error;
}

int LuaScriptVM::functionExists(const std::string& name,
                                bool& exists) {
    int err = 0;
    exists = false;
    lua_getglobal(ls, name.c_str());
    exists = lua_isfunction(ls, -1);
    lua_pop(ls, 1);
    return err;
}
