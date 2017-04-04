/*
 *	EUSearch.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 04/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h
#define __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h

#include <chaos/common/script/AbstractScriptableClass.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                //! forward declaration
                class ScriptableExecutionUnit;
                
                namespace api {
#define EUSW_ADD_DATASET_ATTRIBUTE      "addDatasetAttribute"
#define EUSW_GET_OUTPUT_ATTRIBUTE_VALUE "getOutputAttributeValue"
#define EUSW_SET_OUTPUT_ATTRIBUTE_VALUE "setOutputAttributeValue"
#define EUSW_GET_INPUT_ATTRIBUTE_VALUE  "getInputAttributeValue"
                    
                    //! class taht permit to interact with execution unit and chaos cript framework
                    class EUSearch:
                    public chaos::common::script::TemplatedAbstractScriptableClass<EUSearch> {
                        friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                        
                        //execution unit instances
                        ScriptableExecutionUnit *eu_instance;
                    protected:
                        EUSearch(ScriptableExecutionUnit *_eu_instance);
                        ~EUSearch();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h */
