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
#ifdef CLING_VIRTUAL_MACHINE
#include <chaos/common/script/cling/CPPScriptVM.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos/common/global.h>

#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::script::cling;

using namespace boost::filesystem;

#define INFO   INFO_LOG(CPPScriptVM)
#define IDBG   INFO_LOG(CPPScriptVM)
#define ERR    ERR_LOG(CPPScriptVM)

DEFINE_CLASS_FACTORY(CPPScriptVM, AbstractScriptVM);

CPPScriptVM::CPPScriptVM(const std::string& name):
AbstractScriptVM(name){}

CPPScriptVM::~CPPScriptVM() {}

void CPPScriptVM::init(void *init_data) throw(chaos::CException) {
    path llvm_path;
    char *args[1];
    args[0] = const_cast<char*>(boost::dll::program_location().c_str());

    if(GlobalConfiguration::getInstance()->getRpcImplKVParam().count("llvm_path")) {
        llvm_path = GlobalConfiguration::getInstance()->getRpcImplKVParam()["llvm_path"];
    } else {
        //get local directory
        llvm_path = boost::filesystem::current_path();
        //try to determinate where i'm
        if(llvm_path.leaf().compare("bin") == 0) {
            llvm_path = llvm_path.parent_path();
        }
    }
    
    interpreter.reset(new ::cling::Interpreter(1, args, llvm_path.c_str()));
}

void CPPScriptVM::deinit() throw(chaos::CException) {
    interpreter.reset();
}

int CPPScriptVM::loadScript(const std::string& loadable_script) {
    last_error = 0;
    return last_error;
}

int CPPScriptVM::callFunction(const std::string& function_name,
                              const ScriptInParam& input_parameter,
                              ScriptOutParam& output_parameter) {
    last_error = 0;
    return last_error;
}

int CPPScriptVM::callProcedure(const std::string& function_name,
                               const ScriptInParam& input_parameter) {
    last_error = 0;
    return last_error;
}

int CPPScriptVM::functionExists(const std::string& name,
                                bool& exists) {
    int err = 0;
    exists = false;
    return err;
}
#endif /*CLING_VIRTUAL_MACHINE*/
