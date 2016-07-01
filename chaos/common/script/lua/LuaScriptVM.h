/*
 *	LuaScriptVM.h
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

#ifndef __CHAOSFramework__LuaScriptVM_h
#define __CHAOSFramework__LuaScriptVM_h

#include <chaos/common/script/lua/luna.h>
#include <chaos/common/script/lua/core/lua.hpp>
#include <chaos/common/script/AbstractScriptVM.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>


namespace chaos {
    namespace common {
        namespace script {
            namespace lua {
                class LuaScriptVM;
                
                class ChaosLuaWrapperInterface {
                    friend class LuaScriptVM;
                    ScriptApiCaller *script_caller;
                public:
                    ChaosLuaWrapperInterface(lua_State *ls);
                    ~ChaosLuaWrapperInterface();
                    
                    int callApi(lua_State *ls);
                    
                    static const char className[];
                    static const Luna<ChaosLuaWrapperInterface>::RegType Register[];
                };
                
                //!define the chaos script lua implementation
                DECLARE_CLASS_FACTORY(LuaScriptVM, AbstractScriptVM),
                public LunaAllocationHandler<ChaosLuaWrapperInterface> {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(LuaScriptVM)
                    friend class Luna<ChaosLuaWrapperInterface>;
                    lua_State *ls;
                    LuaScriptVM(const std::string& alias);
                    ~LuaScriptVM();
                protected:
                    void init(void *init_data) throw(chaos::CException);
                    void deinit() throw(chaos::CException);
                    void allocationOf(ChaosLuaWrapperInterface *newAllocatedClass);
                    void deallocationOf(ChaosLuaWrapperInterface *deallocatedClass);
                public:
                    int loadScript(const std::string& loadable_script);
                    
                    int callFunction(const std::string& function_name,
                                     const ScriptInParam& input_parameter,
                                     ScriptOutParam& output_parameter);
                    int callProcedure(const std::string& function_name,
                                      const ScriptInParam& input_parameter);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__LuaScriptVM_h */
