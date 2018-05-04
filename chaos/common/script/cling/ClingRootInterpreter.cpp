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
#include <chaos/common/script/cling/ClingRootInterpreter.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/dll.hpp>

using namespace boost::filesystem;

using namespace chaos::common::script::cling;

#define INFO   INFO_LOG(ClingRootInterpreter)
#define IDBG   INFO_LOG(ClingRootInterpreter)
#define ERR    ERR_LOG(ClingRootInterpreter)

ClingRootInterpreter::ClingRootInterpreter() {
    path llvm_path;
    char *args[1];
    const std::string path = boost::dll::program_location().string();
    args[0] = const_cast<char*>(path.c_str());
    
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
    
    root_interpreter.reset(new ::cling::Interpreter(1, args, llvm_path.c_str()));
    //set the default include file
    ::cling::Interpreter::CompilationResult cr = root_interpreter->declare("#include <chaos/common/data/CDataVariant.h>\n"
                                                                           "#include <chaos/common/script/ScriptApiCaller.h>");
    if(cr == ::cling::Interpreter::kFailure) {
        ERR << "Error including default chaos files";
        root_interpreter.reset();
    }
}

ClingRootInterpreter::~ClingRootInterpreter() {
    root_interpreter.reset();
}

const ::cling::Interpreter& ClingRootInterpreter::getRootInterpreter() {
    return *root_interpreter;
}
#endif /* CLING_VIRTUAL_MACHINE */
