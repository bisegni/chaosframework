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
#include <chaos/common/script/cling/ClingRootInterpreter.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <cling/Interpreter/Transaction.h>
#include <clang/AST/Decl.h>
#include <clang/AST/ASTContext.h>

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

using ScriptProcedurePrototype = int(const ScriptInParam& input_parameter);
using ScriptFunctionPrototype = int(const ScriptInParam& input_parameter, ScriptOutParam& output_parameter);

CPPScriptVM::CPPScriptVM(const std::string& name):
AbstractScriptVM(name){}

CPPScriptVM::~CPPScriptVM() {}

void CPPScriptVM::init(void *init_data) throw(chaos::CException) {
    path llvm_path;
    const char *args[1] = {(char*)"app"};

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
    //set the default include file
    ::cling::Interpreter::CompilationResult cr = interpreter->declare("#include <chaos/common/script/ScriptApiCaller.h>");
    if(cr == ::cling::Interpreter::kFailure) {
        LOG_AND_TROW(ERR, -1, "Error including default chaos files")
    } else {
        //add the caller to script core
        std::ostringstream sstr;
        sstr << "chaos::common::script::ScriptApiCaller& chaos_api = *(chaos::common::script::ScriptApiCaller*)" << std::hex << std::showbase << (size_t)script_caller << ';';
        cr = interpreter->process(sstr.str());
        if(cr == ::cling::Interpreter::kFailure) {
            LOG_AND_TROW(ERR, -2, "Errore during api accessor registration")
        }
//        const ::cling::Transaction* t = interpreter->getLatestTransaction();
//        ::clang::DeclGroupRef d = t->getCurrentLastDecl();
//        ::clang::Decl* d2 = d.getSingleDecl();
//        ::clang::TranslationUnitDecl* tu = d2->getTranslationUnitDecl();
//        ::clang::SourceManager& sm = d2->getASTContext().getSourceManager();
//        ::clang::DeclContext::decl_iterator it;
//        for (it = tu->decls_begin();it != tu->decls_end(); ++it) {
//            ::clang::SourceLocation sl = it->getLocation();
//            if (const clang::VarDecl* vd = clang::dyn_cast<clang::VarDecl>(*it)) {
//                std::cout << sl.printToString(sm) << "\t"
//                << it->getDeclKindName() << "\t"
//                << vd->getName().str() << "\t"
//                << vd->getType().getAsString() << std::endl;
//            }
//        }
    }
}

void CPPScriptVM::deinit() throw(chaos::CException) {
    interpreter.reset();
}

int CPPScriptVM::loadScript(const std::string& loadable_script) {
    last_error = 0;
    CHAOS_ASSERT(interpreter.get());
    
    if(interpreter->declare(loadable_script) != ::cling::Interpreter::kSuccess){
        last_error = -1;
        ERR << "Error processing script";
    }
    return last_error;
}

int CPPScriptVM::callFunction(const std::string& function_name,
                              const ScriptInParam& input_parameter,
                              ScriptOutParam& output_parameter) {
    last_error = 0;
    void* function_addr = interpreter->getAddressOfGlobal(function_name);
    if(function_addr == nullptr) {
        last_error = -1;
        ERR << CHAOS_FORMAT("Function %1% with %2% input paramter and %3% output parameter has not been found",%function_name%input_parameter.size()%output_parameter.size());
    }
    ScriptFunctionPrototype* s_func = ::cling::utils::VoidToFunctionPtr<ScriptFunctionPrototype*>(function_addr);
    return (last_error = s_func(input_parameter,
                                output_parameter));
}

int CPPScriptVM::callProcedure(const std::string& function_name,
                               const ScriptInParam& input_parameter) {
    last_error = 0;
    void* procedure_addr = interpreter->getAddressOfGlobal(function_name);
    if(procedure_addr == nullptr) {
        last_error = -1;
        ERR << CHAOS_FORMAT("Procedure %1% with %2% input paramter has not been found",%function_name%input_parameter.size());
    }
    ScriptProcedurePrototype* s_proc = ::cling::utils::VoidToFunctionPtr<ScriptProcedurePrototype*>(procedure_addr);
    return (last_error = s_proc(input_parameter));
}

int CPPScriptVM::functionExists(const std::string& function_name,
                                bool& exists) {
    int err = 0;
    exists = interpreter->getAddressOfGlobal(function_name) != nullptr;
    return err;
}
#endif /*CLING_VIRTUAL_MACHINE*/
