/*
 *	LuaScriptVM.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 10/05/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/global.h>
#include <chaos/common/script/lua/LuaScriptVM.h>

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

ChaosLuaWrapperInterface::ChaosLuaWrapperInterface(lua_State *L) {
    printf("in ChaosLuaWrapperInterface constructor\n");
}

ChaosLuaWrapperInterface::~ChaosLuaWrapperInterface() {
    printf("in ChaosLuaWrapperInterface destructor\n");
}

int ChaosLuaWrapperInterface::callApi(lua_State *ls) {
    int err = 0;
        //the minimum number of parameter are two (class and api name)
    int nargs = lua_gettop(ls);
    if(nargs < 2) {
            //return error
        lua_pushinteger(ls, -1);
        return 1;
    }

    ScriptInParam   in_param;
    ScriptOutParam  out_param;
        //get api class and function names
    const std::string class_api_name = lua_tostring(ls, 1);
    const std::string api_name = lua_tostring(ls, 2);

        //scan all input parameter
    for (int i=3; i <= nargs; i++) {
        switch(lua_type(ls, 1)) {
            case LUA_TSTRING:
                in_param.push_back(CDataVariant(lua_tostring(ls, i)));
                break;
            case LUA_TNUMBER:
                if(lua_isinteger(ls, i)){
                    in_param.push_back(CDataVariant((int64_t)lua_tointeger(ls, i)));
                } else if (lua_isnumber(ls, i)) {
                    in_param.push_back(CDataVariant(lua_isnumber(ls, i)));
                }
                break;
            case LUA_TBOOLEAN:
                in_param.push_back(CDataVariant(lua_toboolean(ls, i)));
                break;
            default:
                break;
        }
    }

    if((err = script_caller->callScriptApi(class_api_name,
                                           api_name,
                                           in_param,
                                           out_param))) {
        LSVM_ERR << CHAOS_FORMAT("Error calling %1%[%2%] with code %3%", %api_name%api_name%class_api_name%err);
        luaL_argerror(ls, err, CHAOS_FORMAT("Error executing %1%[%2%]", %api_name%api_name).c_str());
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

static const struct luaL_Reg chaos_wrap [] = {
    {"print", lua_print_wrap},
    {NULL, NULL} /* end of array */
};

LuaScriptVM::LuaScriptVM(const std::string& name):
AbstractScriptVM(name),
ls(NULL){

}

LuaScriptVM::~LuaScriptVM() {
    if(ls)lua_close(ls);
}

void LuaScriptVM::init(void *init_data) throw(chaos::CException) {
    ls = luaL_newstate();
    luaopen_os(ls);
    luaopen_base(ls);
    luaopen_math(ls);
    luaopen_bit32(ls);
    luaopen_table(ls);
    luaopen_string(ls);

    lua_getglobal(ls, "_G");
    luaL_setfuncs(ls, chaos_wrap, 0);
    lua_pop(ls, 1);

        //set virtual machine as allocation handler
    Luna<ChaosLuaWrapperInterface>::setHandler(this);
        //register wrapper interface
    Luna<ChaosLuaWrapperInterface>::Register(ls);

}

void LuaScriptVM::deinit() throw(chaos::CException) {
    if(ls)lua_close(ls);
}

void LuaScriptVM::allocationOf(ChaosLuaWrapperInterface *newAllocatedClass) {
        //set the script caller with helper interface
    newAllocatedClass->script_caller = script_caller;
}

void LuaScriptVM::deallocationOf(ChaosLuaWrapperInterface *deallocatedClass) {

}

int LuaScriptVM::loadScript(const std::string& loadable_script) {
    int err = 0;
    if((err = luaL_loadstring(ls, loadable_script.c_str()))){
        LSVM_ERR << CHAOS_FORMAT("Error %1% loading script", %lua_tostring(ls, -1));
    } else if((err = (lua_pcall(ls, 0, 0, 0)))){
        LSVM_ERR << CHAOS_FORMAT("Error %1% compiling script", %lua_tostring(ls, -1));
    }
    return err;
}

int LuaScriptVM::callFunction(const std::string& function_name,
                              uint32_t expected_results_len,
                              const ScriptInParam& input_parameter,
                              ScriptOutParam& output_parameter) {
    int err = 0;
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
    if ((err = lua_pcall(ls, (int)input_parameter.size(), expected_results_len, 0) != 0)) {
        LSVM_ERR << CHAOS_FORMAT("Error %1% calling script function %2%", %lua_tostring(ls, -1)%function_name);
    } else {
            //execution as gone well, we can get the expected result
        for(int idx = 0;
            idx < expected_results_len;
            idx++) {
            switch(lua_type(ls, -1)){
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
        }
    }
    return err;
}