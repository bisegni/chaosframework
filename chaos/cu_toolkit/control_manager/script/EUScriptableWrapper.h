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
                    
                    //! Add an algoritm variable
                    /*!
                     A variable is the way an algorithm can receive or emit data. Theinput variable
                     are automatically fill with the vauels of the attribute dataset of other control unit
                     or execution unit, that are assocaited to the variable using the MDS services. The output
                     variable are vaalorized by algorithm and after the execution step the are sent to the
                     respective CU or EU that are associated to the variable using the MDS services.
                     The api need the following information:
                     
                     alias[string] - the alias of the variable
                     description[string] - the description of the variable
                     type[number] - the type of the value(@chaos::DataType::DataType)
                     direction[number] - is the variable direction @chaos::DataType::DataSetAttributeIOAttribute
                     mandatory[bool] - determinate if the variable is mandatory or no
                     
                     \param input_parameter, parameter  given by the script encoded into !CHAOS variable
                     \param output_parameter, results sent to the script
                     */
                    int addVariable(const common::script::ScriptInParam& input_parameter,
                                    common::script::ScriptOutParam& output_parameter);
                    
                    
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
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__EUScriptableWrapper_h */
