/*
 *	EUDSValueManagement.h
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

#ifndef __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h
#define __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h

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
                    class EUDSValueManagement:
                    public chaos::common::script::TemplatedAbstractScriptableClass<EUDSValueManagement> {
                        friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                        
                        //execution unit instances
                        ScriptableExecutionUnit *eu_instance;
                    protected:
                        EUDSValueManagement(ScriptableExecutionUnit *_eu_instance);
                        ~EUDSValueManagement();
                        
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
                        
                        //! set the value of an output dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         atribute value[as type] - the value of the attribute
                         */
                        int setOutputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                    common::script::ScriptOutParam& output_parameter);
                        
                        //! return the value of an output dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         
                         output attribute
                         atribute value[as type]
                         */
                        int getOutputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                    common::script::ScriptOutParam& output_parameter);
                        
                        //! return the value of an input dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         
                         output attribute
                         atribute value[as type]
                         */
                        int getInputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                   common::script::ScriptOutParam& output_parameter);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h */
