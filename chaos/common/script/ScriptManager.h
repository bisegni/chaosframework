/*
 *	ScriptManager.h
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
                void init(void *init_data) throw(chaos::CException);
                void deinit() throw(chaos::CException);

                    //!entry point of the scirpting for call an exposed api
                int callScriptApi(const std::string& api_class,
                                  const std::string& api_name,
                                  const ScriptInParam& input_parameter,
                                  ScriptOutParam& output_parameter);
            public:
                ScriptManager(const std::string& _script_language);
                ~ScriptManager();
                void registerApiClass(AbstractScriptableClass *api_class);
                void deregisterApiClass(AbstractScriptableClass *api_class);
            };
        }
    }
}

#endif /* __CHAOSFramework__ScriptManager_h */
