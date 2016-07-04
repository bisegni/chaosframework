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
                
#define EUSW_ADD_DATASET_ATTRIBUTE "addDatasetAttrbiute"
#define EUSW_GET_DOMAIN_ATTRIBUTE_VALUE "getDomainAttributeValue"
                
                //! forward declaration
                class ScriptableExecutionUnit;
                
                //! class taht permit to interact with execution unit and chaos cript framework
                class EUScriptableWrapper:
                public chaos::common::script::TemplatedAbstractScriptableClass<EUScriptableWrapper> {
                    friend class ScriptableExecutionUnit;
                    
                    //execution unit instances
                    ScriptableExecutionUnit *eu_instance;
                protected:
                    EUScriptableWrapper(ScriptableExecutionUnit *_eu_instance);
                    ~EUScriptableWrapper();
                    
                    //! Add an attribute to the dataset
                    /*!
                     Add an attribute to the execution unit dataset, that represent the thirth type of data managed by EU.
                     The eu dataset have the same meaning of the dataset of the SCCU or RTCU.
                     
                     alias[string] is the name of the dataset attribute
                     description[string] - is the description of the attribute
                     type[number] - the type of the attribute(@chaos::DataType::DataType)
                     direction[number] - is the attribute direction @chaos::DataType::DataSetAttributeIOAttribute
                     */
                    int addDatasetAttribute(const common::script::ScriptInParam& input_parameter,
                                            common::script::ScriptOutParam& output_parameter);
                    
                    //! return the value of an dataset attribute
                    /*!
                     input parameter
                     domain[number] - 0,DOMAIN_OUTPUT 1,DOMAIN_INPUT 2,DOMAIN_CUSTOM 3,DOMAIN_SYSTEM
                     attribute_name[string] - the name of the attribute
                     
                     output attribute
                     atribute type[number] a value that represent the enum @chaos::DataType::DataType
                     atribute value[as type]
                     */
                    int getDomainAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                common::script::ScriptOutParam& output_parameter);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__EUScriptableWrapper_h */
