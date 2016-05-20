/*
 *	ScriptableExecutionUnit.h
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

#ifndef __CHAOSFramework__ScriptableExecutionUnit_h
#define __CHAOSFramework__ScriptableExecutionUnit_h

#include <chaos/common/script/ScriptManager.h>

#include <chaos/cu_toolkit/control_manager/script/EUScriptableWrapper.h>
#include <chaos/cu_toolkit/control_manager/AbstractExecutionUnit.h>

#define SEU_SCRIPT_LANGUAGE     "script_language"
#define SEU_SCRIPT_CONTENT      "script_content"

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {

#define SEU_DEFINE_DATASET  "defineDataset"
#define SEU_ALGORITHM_STEP  "algorithmStep"
                
                    //! this class implementa an execution unit defined by a script
                /*!
                 the script and language is submitted at load time with a json ith the following template
                 {
                 "script_language":"lua",
                 "script_content":"the text of the script"
                 }
                 */
                class ScriptableExecutionUnit:
                public AbstractExecutionUnit {
                    //! the language to be used for the script
                    std::string script_language;
                    
                    //! the content of the script
                    std::string script_content;
                    
                    //!scrip manger instance initilizated at eu init time
                    std::auto_ptr<common::script::ScriptManager> script_manager;
                    
                    //! class taht wrap the execution uni to script
                    EUScriptableWrapper scriptable_wrapper;
                public:
                    /*! default constructor
                     \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
                     \param _execution_unit_drivers driver information
                     */
                    ScriptableExecutionUnit(const std::string& _execution_unit_id,
                                            const std::string& _execution_unit_param);
                    /*!
                     Parametrized constructor
                     \param _execution_unit_id unique id for the control unit
                     \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
                     \param _execution_unit_drivers driver information
                     */
                    ScriptableExecutionUnit(const std::string& _execution_unit_id,
                                            const std::string& _execution_unit_param,
                                            const ControlUnitDriverList& _execution_unit_drivers);

                        //!default destructor
                    ~ScriptableExecutionUnit();
                    
                protected:
                    //! inherited method
                    void unitDefineActionAndDataset() throw(CException);
                    
                    //! inherited method
                    void unitInit() throw(CException);
                    
                    //! inherited method
                    void unitStart() throw(CException);
                    
                    //! inherited method
                    void unitRun() throw(CException);
                    
                    //! inherited method
                    void unitStop() throw(CException);
                    
                    //! inherited method
                    void unitDeinit() throw(CException);
                    
                    //! inherithed method
                    void unitUndefineActionAndDataset() throw(CException);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__ScriptableExecutionUnit_h */
