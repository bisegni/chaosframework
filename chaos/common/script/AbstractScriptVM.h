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
                int last_error;
                std::string last_error_message;
                ScriptApiCaller *script_caller;
            public:
                AbstractScriptVM(const std::string& service_name);
                virtual ~AbstractScriptVM();
                void setCaller(ScriptApiCaller *_script_caller);
                ScriptApiCaller *getCaller();
                virtual int loadScript(const std::string& loadable_script) = 0;

                virtual int callFunction(const std::string& function_name,
                                         const ScriptInParam& input_parameter,
                                         ScriptOutParam& output_parameter) = 0;
                
                virtual int callProcedure(const std::string& function_name,
                                          const ScriptInParam& input_parameter) = 0;
                
                virtual int functionExists(const std::string& name, bool& exists) = 0;
                
                const int getLastError() const;
                const std::string& getLastErrorMessage() const;
            };

        }
    }
}

#endif /* __CHAOSFramework__AbstractScriptVM_h */
