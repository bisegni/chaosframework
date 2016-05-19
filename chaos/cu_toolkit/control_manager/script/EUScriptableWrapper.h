/*
 *	EUScriptableWrapper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 13/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__EUScriptableWrapper_h
#define __CHAOSFramework__EUScriptableWrapper_h

#include <chaos/common/script/AbstractScriptableClass.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                    //! forward declaration
                class ScriptableExecutionUnit;

                    //! class taht permit to interact with execution unit and chaos cript framework
                class EUScriptableWrapper:
                public chaos::common::script::TemplatedAbstractScriptableClass<EUScriptableWrapper> {
                    friend class ScriptableExecutionUnit;
                protected:
                    EUScriptableWrapper();
                    ~EUScriptableWrapper();
                public:
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__EUScriptableWrapper_h */
