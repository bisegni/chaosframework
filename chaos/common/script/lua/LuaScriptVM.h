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

#ifndef __CHAOSFramework__LuaScriptVM_h
#define __CHAOSFramework__LuaScriptVM_h

#include <chaos/common/chaos_types.h>
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
                    int functionExists(const std::string& name,
                                       bool& exists);
                };
            }
        }
    }
}
#endif /* __CHAOSFramework__LuaScriptVM_h */
