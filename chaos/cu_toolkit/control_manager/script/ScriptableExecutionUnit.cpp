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

#include <json/json.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::script;
using namespace chaos::common::utility;
using namespace chaos::cu::control_manager::script;

#define SEU_LAPP    INFO_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "
#define SEU_DBG     DBG_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "
#define SEU_LERR    ERR_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "

/*! default constructor
 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
 \param _execution_unit_drivers driver information
 */
ScriptableExecutionUnit::ScriptableExecutionUnit(const std::string& _execution_unit_id,
                                                 const std::string& _execution_unit_param):
AbstractExecutionUnit(_execution_unit_id,
                      _execution_unit_param),
scriptable_wrapper(this){}

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
                      _execution_unit_drivers),
scriptable_wrapper(this){}

//!default destructor
ScriptableExecutionUnit::~ScriptableExecutionUnit() {}

void ScriptableExecutionUnit::unitDefineActionAndDataset() throw(CException) {
    //scan load parameter
    CHAOS_LASSERT_EXCEPTION((getCUParam().size() > 0), SEU_LERR, -1, "NO JSON script information has been set at load time");
    ScriptOutParam  out_param;
    Json::Reader    json_reader;
    Json::Value     json_params;
    
    //scan json option
    if(json_reader.parse(getCUParam(), json_params)) {
        const Json::Value& _script_language = json_params[SEU_SCRIPT_LANGUAGE];
        const Json::Value&  _script_content = json_params[SEU_SCRIPT_CONTENT];
        CHAOS_LASSERT_EXCEPTION((_script_language.isNull() && _script_content.isString()), SEU_LERR, -2, "The script language is not defined (or not a string) into load parameter");
        CHAOS_LASSERT_EXCEPTION((_script_content.isNull() && _script_content.isString()), SEU_LERR, -3, "The script content is not defined (or not a string) into load parameter");
        if(_script_language.isNull())
            //we nned to crasch in some situation
            script_language = _script_language.asString();
        
        //we nned to crasch in some situation
        script_content = _script_content.asString();
        
    } else {
        LOG_AND_TROW(SEU_LERR, -4, "Error decoding JSON load parameter");
    }
    
    //allcoate script manager or language
    
    SEU_LAPP << CHAOS_FORMAT("Create script manage for '%1%' language!", %script_language);
    script_manager.reset(new ScriptManager(script_language));
    InizializableService::initImplementation(script_manager.get(),
                                             NULL,
                                             "ScriptManager",
                                             __PRETTY_FUNCTION__);
    SEU_LAPP << "Register api";
    script_manager->registerApiClass(&scriptable_wrapper);
    
    //!load script within the virtual machine
    SEU_LAPP << "Try to load the script";
    if(script_manager->getVirtualMachine()->loadScript(script_content)) {
        LOG_AND_TROW(SEU_LERR, -5, "Error loading script into virtual machine");
    }
    
    //execute script method that define the dataset

    if(script_manager->getVirtualMachine()->callFunction(SEU_DEFINE_DATASET,
                                                         out_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -6, "Error calling function %1% of the script(it maybe not implemented)", %SEU_DEFINE_DATASET);
    }
}

void ScriptableExecutionUnit::unitInit() throw(CException) {
    //at this time we can initialize the
    
}

void ScriptableExecutionUnit::unitStart() throw(CException) {
    
}

void ScriptableExecutionUnit::unitStop() throw(CException) {
    
}

void ScriptableExecutionUnit::unitDeinit() throw(CException) {
    
}

void ScriptableExecutionUnit::unitRun() throw(CException) {
    
}

void ScriptableExecutionUnit::unitUndefineActionAndDataset() throw(CException) {
    if(script_manager.get() != NULL) {
        InizializableService::deinitImplementation(script_manager.get(),
                                                   "ScriptManager",
                                                   __PRETTY_FUNCTION__);
        //clear memory
        script_manager.reset();
    }
    
}