/*
 *	EULiveManagment.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/05/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__B03D4FF_E4B9_48D3_8F83_909066AD570A_EULiveManagment_h
#define __CHAOSFramework__B03D4FF_E4B9_48D3_8F83_909066AD570A_EULiveManagment_h

#include <chaos/common/script/AbstractScriptableClass.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                //! forward declaration
                class ScriptableExecutionUnit;
                namespace api {
                    //! class taht permit to interact with execution unit and chaos cript framework
                    class EULiveManagment:
                    public chaos::common::script::TemplatedAbstractScriptableClass<EULiveManagment> {
                        friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                        
                        //execution unit instances
                        ScriptableExecutionUnit *eu_instance;
                    public:
                        ~EULiveManagment();
                    protected:
                        EULiveManagment(ScriptableExecutionUnit *_eu_instance);
                        
                        //fetch new dataset for node uid
                        /*!
                         fetch form live a new dataset for one or more device id
                         
                         node unique id[string] is the unique id of the node that we ent to get a new dataset form live data
                         
                         */
                        int fetch(const chaos::common::script::ScriptInParam& input_parameter,
                                  chaos::common::script::ScriptOutParam& output_parameter);
                    
                        //! Get a new dataset from live data and return attribute values
                        /*!
                         Return a series of value associated to a data for a node id
                         
                         node unique id[string] is the unique id of the node that we ent to get a new dataset form live data
                         domain[string] - is the domain for wich we want to get the data
                         attribute name[string] - 
                         */
                        int getValueForKey(const chaos::common::script::ScriptInParam& input_parameter,
                                           chaos::common::script::ScriptOutParam& output_parameter);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__B03D4FF_E4B9_48D3_8F83_909066AD570A_EULiveManagment_h */
