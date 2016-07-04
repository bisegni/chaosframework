/*
 *	AbstractScriptVM.h
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

#ifndef __CHAOSFramework__AbstractScriptVM_h
#define __CHAOSFramework__AbstractScriptVM_h

#include <chaos/common/script/script_types.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/script/ScriptApiCaller.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
    namespace common{
        namespace script {

                //!base class for script virtual machine
            class AbstractScriptVM:
            public common::utility::NamedService,
            public chaos::common::utility::InizializableService {
            protected:
                ScriptApiCaller *script_caller;
            public:
                AbstractScriptVM(const std::string& service_name);
                virtual ~AbstractScriptVM();
                void setCaller(ScriptApiCaller *_script_caller);

                virtual int loadScript(const std::string& loadable_script) = 0;

                virtual int callFunction(const std::string& function_name,
                                         const ScriptInParam& input_parameter,
                                         ScriptOutParam& output_parameter) = 0;
                
                virtual int callProcedure(const std::string& function_name,
                                          const ScriptInParam& input_parameter) = 0;
                
                virtual int functionExists(const std::string& name, bool& exists) = 0;
            };

        }
    }
}

#endif /* __CHAOSFramework__AbstractScriptVM_h */
