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

#ifndef __CHAOSFramework__ScriptManager_h
#define __CHAOSFramework__ScriptManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/script/ScriptApiCaller.h>
#include <chaos/common/script/AbstractScriptVM.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common{
        namespace script {

            class AbstractScriptableClass;

            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, AbstractScriptableClass*, MapRegisteredApiClass)
            
                //! define the abstraction of a scriptable class
            /*!
             Evrey sublcass of this one can expose method that can be callable form
             a script language.
             */
            class ScriptManager:
            public ScriptApiCaller,
            public chaos::common::utility::InizializableService {
                boost::shared_mutex map_mutex;
                const std::string vm_name;
                const std::string script_language;
                
                AbstractScriptVM *script_vm;
                MapRegisteredApiClass map_api_class;
            protected:
                void init(void *init_data);
                void deinit();

                    //!entry point of the scripting for call an exposed api
                int callScriptApi(const std::string& api_class,
                                  const std::string& api_name,
                                  const ScriptInParam& input_parameter,
                                  ScriptOutParam& output_parameter);
            public:
                ScriptManager(const std::string& _script_language);
                ~ScriptManager();
                void registerApiClass(AbstractScriptableClass& api_class);
                void deregisterApiClass(AbstractScriptableClass& api_class);
                AbstractScriptVM *getVirtualMachine();
            };
        }
    }
}

#endif /* __CHAOSFramework__ScriptManager_h */
