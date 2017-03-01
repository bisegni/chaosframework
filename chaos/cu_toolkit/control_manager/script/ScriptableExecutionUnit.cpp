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

#include <chaos/common/bson/util/base64.h>

#include <json/json.h>

using namespace chaos::common::script;
using namespace chaos::common::utility;
using namespace chaos::cu::control_manager::script;

#define SEU_LAPP    INFO_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "
#define SEU_DBG     DBG_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "
#define SEU_LERR    ERR_LOG(ScriptableExecutionUnit) << getDeviceID() << " - "

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(ScriptableExecutionUnit)

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


void ScriptableExecutionUnit::addAttributeToDataSet(const std::string& attribute_name,
                                                    const std::string& attribute_description,
                                                    DataType::DataType attribute_type,
                                                    DataType::DataSetAttributeIOAttribute attribute_direction,
                                                    uint32_t maxSize) {
    AbstractExecutionUnit::addAttributeToDataSet(attribute_name,
                                                 attribute_description,
                                                 attribute_type,
                                                 attribute_direction);
}

void ScriptableExecutionUnit::unitDefineActionAndDataset() throw(CException) {
    int err = 0;
    bool exists = false;
    //clear bitet for implemented lagorithm handler
    alghorithm_handler_implemented.reset();
    
    //scan load parameter
    CHAOS_LASSERT_EXCEPTION((getCUParam().size() > 0), SEU_LERR, -1, "NO JSON script information has been set at load time");
    CHAOS_LASSERT_EXCEPTION(isCUParamInJson(), SEU_LERR, -2, "Load parameter are not a json document");
    
    //!get root json element
    const Json::Value&     json_params = getCUParamJsonRootElement();
    
    //scan json option
    if(json_params.isNull() == false) {
        const Json::Value& _script_language = json_params[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE];
        const Json::Value&  _script_content = json_params[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT];
        const Json::Value&  _script_b64_dataset = json_params[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION];
        
        CHAOS_LASSERT_EXCEPTION(((_script_language.isNull() == false) && _script_content.isString()),
                                SEU_LERR,
                                -2,
                                "The script language is not defined (or not a string) into load parameter");
        CHAOS_LASSERT_EXCEPTION(((_script_content.isNull() == false)  &&
                                 _script_content.isString()),
                                SEU_LERR,
                                -3,
                                "The script content is not defined (or not a string) into load parameter");
        
        //we nned to crasch in some situation
        script_language = _script_language.asString();
        
        //we nned to crasch in some situation
        script_content = _script_content.asString();
        
        //set the script dataset
        if(_script_b64_dataset.isNull() == false &&
           _script_b64_dataset.isString()) {
            const std::string decoded_bson_dataset = bson::base64::decode(_script_b64_dataset.asString());
            CDataWrapper script_dataset(decoded_bson_dataset.c_str());
            addAttributeToDataSetFromDataWrapper(script_dataset);
        }

    } else {
        LOG_AND_TROW(SEU_LERR, -4, "Error decoding JSON load parameter");
    }
    
    //allocate script manager or language
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
    
    //check for implemented handler
    if((err = script_manager->getVirtualMachine()->functionExists(SEU_ALGORITHM_LAUNCH, exists)) ){
        LOG_AND_TROW(SEU_LERR,
                     -3,
                     CHAOS_FORMAT("Error checking the presence of the function %1%",%SEU_ALGORITHM_LAUNCH));
    }
    alghorithm_handler_implemented[0] = exists;
    
    if((err = script_manager->getVirtualMachine()->functionExists(SEU_ALGORITHM_START, exists)) ){
        LOG_AND_TROW(SEU_LERR,
                     -4,
                     CHAOS_FORMAT("Error checking the presence of the function %1%",%SEU_ALGORITHM_START));
    }
    alghorithm_handler_implemented[1] = exists;
    
    if((err = script_manager->getVirtualMachine()->functionExists(SEU_ALGORITHM_STEP, exists)) ){
        LOG_AND_TROW(SEU_LERR,
                     -5,
                     CHAOS_FORMAT("Error checking the presence of the function %1%",%SEU_ALGORITHM_STEP));
    }
    alghorithm_handler_implemented[2] = exists;
    
    if((err = script_manager->getVirtualMachine()->functionExists(SEU_ALGORITHM_STOP, exists)) ){
        LOG_AND_TROW(SEU_LERR,
                     -6,
                     CHAOS_FORMAT("Error checking the presence of the function %1%",%SEU_ALGORITHM_STOP));
    }
    alghorithm_handler_implemented[3] = exists;
    
    if((err = script_manager->getVirtualMachine()->functionExists(SEU_ALGORITHM_END, exists)) ){
        LOG_AND_TROW(SEU_LERR,
                     -7,
                     CHAOS_FORMAT("Error checking the presence of the function %1%",%SEU_ALGORITHM_END));
    }
    alghorithm_handler_implemented[4] = exists;
    
    
    //    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_SETUP,
    //                                                          in_param)) {
    //        LOG_AND_TROW_FORMATTED(SEU_LERR, -6, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_SETUP);
    //    }
}

void ScriptableExecutionUnit::executeAlgorithmLaunch() throw (CException) {
    if(!alghorithm_handler_implemented[0]) return;
    ScriptInParam input_param;
    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_LAUNCH,
                                                          input_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -1, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_LAUNCH);
    }
}

void ScriptableExecutionUnit::executeAlgorithmStart() throw (CException) {
    if(!alghorithm_handler_implemented[1]) return;
    ScriptInParam input_param;
    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_START,
                                                          input_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -1, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_START);
    }
}


void ScriptableExecutionUnit::executeAlgorithmStep(uint64_t step_delay_time) throw (CException) {
    if(!alghorithm_handler_implemented[2]) return;
    ScriptInParam input_param;
    ScriptOutParam output_param;
    
    //add step delay time
    input_param.push_back(CDataVariant((int64_t)step_delay_time));
    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_STEP,
                                                          input_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -1, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_STEP);
    }
}

void ScriptableExecutionUnit::executeAlgorithmStop() throw (CException) {
    if(!alghorithm_handler_implemented[3]) return;
    ScriptInParam input_param;
    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_STOP,
                                                          input_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -1, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_STOP);
    }
}

void ScriptableExecutionUnit::executeAlgorithmEnd() throw (CException) {
    if(!alghorithm_handler_implemented[4]) return;
    ScriptInParam input_param;
    if(script_manager->getVirtualMachine()->callProcedure(SEU_ALGORITHM_END,
                                                          input_param)) {
        LOG_AND_TROW_FORMATTED(SEU_LERR, -1, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_END);
    }
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
