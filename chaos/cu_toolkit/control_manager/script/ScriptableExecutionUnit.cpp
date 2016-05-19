/*
 *	ScriptableExecutionUnit.cpp
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

#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::cu::control_manager::script;

/*! default constructor
 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
 \param _execution_unit_drivers driver information
 */
ScriptableExecutionUnit::ScriptableExecutionUnit(const std::string& _execution_unit_id,
                                                 const std::string& _execution_unit_param):
AbstractExecutionUnit(_execution_unit_id,
                      _execution_unit_param){}

/*!
 Parametrized constructor
 \param _execution_unit_id unique id for the control unit
 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
 \param _execution_unit_drivers driver information
 */
ScriptableExecutionUnit::ScriptableExecutionUnit(const std::string& _execution_unit_id,
                                                 const std::string& _execution_unit_param,
                                                 const ControlUnitDriverList& _execution_unit_drivers):
AbstractExecutionUnit(_execution_unit_id,
                      _execution_unit_param,
                      _execution_unit_drivers){}

    //!default destructor
ScriptableExecutionUnit::~ScriptableExecutionUnit() {
    
}