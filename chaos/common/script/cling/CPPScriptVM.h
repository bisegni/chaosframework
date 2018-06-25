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

#ifndef CPPScriptVM_hpp
#define CPPScriptVM_hpp
#ifdef CLING_VIRTUAL_MACHINE

#include <chaos/common/chaos_types.h>
#include <chaos/common/script/AbstractScriptVM.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <cling/Interpreter/Interpreter.h>
#include <cling/Interpreter/Value.h>
#include <cling/Utils/Casting.h>

namespace chaos {
    namespace common {
        namespace script {
            namespace cling {
                //!define the chaos script c++ implementation using cern cling
                DECLARE_CLASS_FACTORY(CPPScriptVM, AbstractScriptVM) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(CPPScriptVM)
                    bool script_loaded;
                    //cling interpreter
                    ChaosUniquePtr<::cling::Interpreter> interpreter;
                    CPPScriptVM(const std::string& alias);
                    ~CPPScriptVM();
                    
                    void initNewInterpreter();
                protected:
                    void init(void *init_data) throw(chaos::CException);
                    void deinit() throw(chaos::CException);
                public:
                    int loadScript(const std::string& loadable_script);
                    
                    int callFunction(const std::string& function_name,
                                     const ScriptInParam& input_parameter,
                                     ScriptOutParam& output_parameter);
                    int callProcedure(const std::string& function_name,
                                      const ScriptInParam& input_parameter);
                    int functionExists(const std::string& function_name,
                                       bool& exists);
                };
            }
        }
    }
}
#endif /*CLING_VIRTUAL_MACHINE*/
#endif /* CPPScriptVM_hpp */
