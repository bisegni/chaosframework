/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUAbstractApiPlugin.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginApiWrapper.h>

#include <chaos/common/bson/util/base64.h>
#include <chaos/common/exception/MetadataLoggingCException.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos_service_common/data/data.h>

#include <json/json.h>

using namespace chaos::common::data;
using namespace chaos::common::plugin;

using namespace chaos::common::script;

using namespace chaos::common::utility;
using namespace chaos::common::exception;
using namespace chaos::common::metadata_logging;
using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api::plugin;

#define SEU_LAPP    INFO_LOG_1_P(ScriptableExecutionUnit, getDeviceID())
#define SEU_DBG     DBG_LOG_1_P(ScriptableExecutionUnit, getDeviceID())
#define SEU_LERR    ERR_LOG_1_P(ScriptableExecutionUnit, getDeviceID())

#define SEU_ALGORITHM_LAUNCH        "algorithmLaunch"
#define SEU_ALGORITHM_START         "algorithmStart"
#define SEU_ALGORITHM_STEP          "algorithmStep"
#define SEU_ALGORITHM_STOP          "algorithmStop"
#define SEU_ALGORITHM_END           "algorithmEnd"
#define SEU_INPUT_ATTRIBUTE_CHANGED "inputAttributeChanged"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(ScriptableExecutionUnit)

/*! default constructor
 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
 \param _execution_unit_drivers driver information
 */
ScriptableExecutionUnit::ScriptableExecutionUnit(const std::string &_execution_unit_id,
                                                 const std::string &_execution_unit_param) :
AbstractExecutionUnit(CUType::SEXUT,
                      _execution_unit_id,
                      _execution_unit_param) {}

/*!
 Parametrized constructor
 \param _execution_unit_id unique id for the control unit
 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
 \param _execution_unit_drivers driver information
 */
ScriptableExecutionUnit::ScriptableExecutionUnit(const std::string &_execution_unit_id,
                                                 const std::string &_execution_unit_param,
                                                 const ControlUnitDriverList &_execution_unit_drivers) :
AbstractExecutionUnit(CUType::SEXUT,
                      _execution_unit_id,
                      _execution_unit_param,
                      _execution_unit_drivers) {}

//!default destructor
ScriptableExecutionUnit::~ScriptableExecutionUnit() {}

void
ScriptableExecutionUnit::addAttributeToDataSet(const std::string &attribute_name,
                                               const std::string &attribute_description,
                                               DataType::DataType attribute_type,
                                               DataType::DataSetAttributeIOAttribute attribute_direction,
                                               uint32_t maxSize) {
    AbstractExecutionUnit::addAttributeToDataSet(attribute_name,
                                                 attribute_description,
                                                 attribute_type,
                                                 attribute_direction);
}

void ScriptableExecutionUnit::registerApi() {
    CHAOS_ASSERT(script_manager().get());
    //register normal api
    int err = 0;
    for(VectorApiClassIterator it = api_classes.begin(),
        end = api_classes.end();
        it != end;
        it++) {
          SEU_LAPP << "Inizializing api " << (*it)->getClassName();
        if((err = (*it)->init()) == 0) {
            script_manager()->registerApiClass(*(*it));
        } else {
            SEU_LERR << CHAOS_FORMAT("Error %1% initilizing plugin api %2%", %err%(*it)->getClassName());
        }
    }
}

void ScriptableExecutionUnit::unregisterApi() {
    CHAOS_ASSERT(script_manager().get());
    //deregister normal api
    for(VectorApiClassIterator it = api_classes.begin(),
        end = api_classes.end();
        it != end;
        it++) {
        (*it)->deinit();
        script_manager()->deregisterApiClass(*(*it));
    }
}

#define LOG_AND_TROW_EX(log, num, msg)\
log << "("<<num<<") " << msg;\
throw MetadataLoggingCException(getDeviceID(), num, msg, __PRETTY_FUNCTION__);

void ScriptableExecutionUnit::unitDefineActionAndDataset() throw(CException) {
    int err = 0;
    bool exists = false;

    //define custom action
    addActionDescritionInstance<ScriptableExecutionUnit>(this,
                                                         &ScriptableExecutionUnit::updateScriptSource,
                                                         "updateScriptSource",
                                                         "Update the source of the script");

    ChaosStringVector defined_input_attribute;
    //clear bitet for implemented lagorithm handler
    alghorithm_handler_implemented.reset();

    //scan load parameter
    CHAOS_LASSERT_EXCEPTION((getCUParam().size() > 0),
                            SEU_LERR,
                            -1,
                            "NO JSON script information has been set at load time");
    CHAOS_LASSERT_EXCEPTION(isCUParamInJson(), SEU_LERR, -2, "Load parameter are not a json document");

    //!get root json element
    const Json::Value &json_params = getCUParamJsonRootElement();

    //scan json option
    if(json_params.isNull() == false) {
        const Json::Value
        &_script_language = json_params[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE];
        const Json::Value
        &_script_content = json_params[ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT];
        const Json::Value
        &_script_b64_dataset = json_params[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION];

        CHAOS_LASSERT_EXCEPTION(((_script_language.isNull() == false) && _script_content.isString()),
                                SEU_LERR,
                                -2,
                                "The script language is not defined (or not a string) into load parameter");
        CHAOS_LASSERT_EXCEPTION(((_script_content.isNull() == false) &&
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

    }
    else {
        LOG_AND_TROW(SEU_LERR, -4, "Error decoding JSON load parameter");
    }

    //get all input attribut to registr the handler for change event
    getDatasetAttributesName(DataType::Input,
                             defined_input_attribute);
    for(ChaosStringVectorIterator it = defined_input_attribute.begin(),
        end = defined_input_attribute.end();
        it != end;
        it++) {
        addVariantHandlerOnInputAttributeName<ScriptableExecutionUnit>(this,
                                                                       &ScriptableExecutionUnit::updatedInputDataset,
                                                                       *it);
    }

    //allocate script manager or language
    SEU_LAPP << CHAOS_FORMAT("Create script manager for '%1%' language!", %script_language);
    script_manager().reset(new ScriptManager(script_language));
    InizializableService::initImplementation(script_manager().get(),
                                             NULL,
                                             "ScriptManager",
                                             __PRETTY_FUNCTION__);
    SEU_LAPP << "Register api";
    api_classes.push_back(ApiClassShrdPtr(new api::EUSearch(this)));
    api_classes.push_back(ApiClassShrdPtr(new api::EUDSValueManagement(this)));
    api_classes.push_back(ApiClassShrdPtr(new api::EULiveManagment(this)));

    if(GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_PLUGIN_ENABLE)) {
        ChaosStringVector publish_api;
        //add api form pugin
        PluginManager::getInstance()->getRegisterdPluginForSubclass("chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin", publish_api);

        for(ChaosStringVectorIterator it = publish_api.begin(),
            end = publish_api.end();
            it != end;
            it++) {
            ChaosSharedPtr<api::plugin::EUAbstractApiPlugin> api_instance(PluginManager::getInstance()->getPluginInstanceBySubclassAndName<api::plugin::EUAbstractApiPlugin>("chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin",*it).release());
            if(api_instance.get() == NULL) continue;
            SEU_LAPP << CHAOS_FORMAT("Add API '%1%' form plugin!", %api_instance->getApiName());
            api_classes.push_back(ApiClassShrdPtr(new EUPluginApiWrapper(this, api_instance)));
        }
    }
    registerApi();

    //!load script within the virtual machine
    SEU_LAPP << "Try to load the script";
    if(script_manager()->getVirtualMachine()->loadScript(script_content)) {
        LOG_AND_TROW_EX(SEU_LERR, -5, "Error loading script into virtual machine");
    }

    //check for implemented handler
    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_ALGORITHM_LAUNCH, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -3,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_ALGORITHM_LAUNCH));
    }
    alghorithm_handler_implemented[0] = exists;

    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_ALGORITHM_START, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -4,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_ALGORITHM_START));
    }
    alghorithm_handler_implemented[1] = exists;

    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_ALGORITHM_STEP, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -5,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_ALGORITHM_STEP));
    }
    alghorithm_handler_implemented[2] = exists;

    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_ALGORITHM_STOP, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -6,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_ALGORITHM_STOP));
    }
    alghorithm_handler_implemented[3] = exists;

    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_ALGORITHM_END, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -7,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_ALGORITHM_END));
    }
    alghorithm_handler_implemented[4] = exists;

    if((err = script_manager()->getVirtualMachine()->functionExists(SEU_INPUT_ATTRIBUTE_CHANGED, exists))) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -7,
                        CHAOS_FORMAT("Error checking the presence of the function %1%", % SEU_INPUT_ATTRIBUTE_CHANGED));
    }
    alghorithm_handler_implemented[5] = exists;
    //    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_SETUP,
    //                                                          in_param)) {
    //        LOG_AND_TROW_FORMATTED(SEU_LERR, -6, "Error calling function %1% of the script(it maybe not implemented)", %SEU_ALGORITHM_SETUP);
    //    }
}

void ScriptableExecutionUnit::executeAlgorithmLaunch() throw(CException) {
    if(!alghorithm_handler_implemented[0])
        return;
    LockableScriptManagerReadLock rl = script_manager.getReadLockObject();
    ScriptInParam input_param;
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_LAUNCH,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());
    }
}

void ScriptableExecutionUnit::executeAlgorithmStart() throw(CException) {
    if(!alghorithm_handler_implemented[1])
        return;
    LockableScriptManagerReadLock rl = script_manager.getReadLockObject();
    ScriptInParam input_param;
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_START,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());
    }
}

void ScriptableExecutionUnit::executeAlgorithmStep(uint64_t step_delay_time) throw(CException) {
    if(!alghorithm_handler_implemented[2])
        return;
    LockableScriptManagerReadLock rl = script_manager.getReadLockObject();
    ScriptInParam input_param;
    ScriptOutParam output_param;

    //add step delay time
    input_param.push_back(CDataVariant((int64_t)step_delay_time));
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_STEP,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());
    }
}

void ScriptableExecutionUnit::executeAlgorithmStop() throw(CException) {
    if(!alghorithm_handler_implemented[3])
        return;
    LockableScriptManagerReadLock rl = script_manager.getReadLockObject();
    ScriptInParam input_param;
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_STOP,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());;
    }
}

void ScriptableExecutionUnit::executeAlgorithmEnd() throw(CException) {
    if(!alghorithm_handler_implemented[4])
        return;
    LockableScriptManagerReadLock rl = script_manager.getReadLockObject();
    ScriptInParam input_param;
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_END,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());
    }
}

void ScriptableExecutionUnit::unitUndefineActionAndDataset() throw(CException) {
    SEU_LAPP << "Unregister api";
    unregisterApi();

    if(script_manager().get() != NULL) {
        InizializableService::deinitImplementation(script_manager().get(),
                                                   "ScriptManager",
                                                   __PRETTY_FUNCTION__);
        //clear memory
        script_manager().reset();
    }
}

bool ScriptableExecutionUnit::updatedInputDataset(const std::string &attribute_name,
                                                  const chaos::common::data::CDataVariant &value) {
    SEU_DBG << CHAOS_FORMAT("Signal for %1% input dataset attribute with value %2%", %attribute_name%value.asString());
    if(!alghorithm_handler_implemented[5])
        return false;
    LockableScriptManagerWriteLock rl = script_manager.getWriteLockObject();

    bool managed = true;
    ScriptInParam input_param;
    input_param.push_back(CDataVariant(attribute_name));
    input_param.push_back(value);
    if(script_manager()->getVirtualMachine()->callProcedure(SEU_INPUT_ATTRIBUTE_CHANGED,
                                                            input_param)) {
        LOG_AND_TROW_EX(SEU_LERR,
                        script_manager()->getVirtualMachine()->getLastError(),
                        script_manager()->getVirtualMachine()->getLastErrorMessage());
        managed = false;
    }
    return managed;
}

CDataWrapper * ScriptableExecutionUnit::updateScriptSource(CDataWrapper *data_pack,
                                                           bool &detachParam) throw(CException) {
    ScriptInParam input_param;
    chaos::service_common::data::script::ScriptSDWrapper sdw(data_pack);
    LockableScriptManagerWriteLock rl = script_manager.getWriteLockObject();

    if(sdw().script_description.language.compare(script_language) != 0) {
        LOG_AND_TROW_EX(SEU_LERR,
                        -1,
                        "New script language differ from unit virtual machine type");
    }
    //store new script
    script_content = sdw().script_content;
    if(script_manager()->getVirtualMachine()->loadScript(script_content)) {
        LOG_AND_TROW_EX(SEU_LERR, -2, "Error loading script into virtual machine");
    }

    //now we need to simulate the init and start operation
    switch(getServiceState()) {
        case CUStateKey::INIT:
            if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_LAUNCH,
                                                                    input_param)) {
                LOG_AND_TROW_EX(SEU_LERR,
                                script_manager()->getVirtualMachine()->getLastError(),
                                script_manager()->getVirtualMachine()->getLastErrorMessage());
            }
            break;
        case CUStateKey::START:
            if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_LAUNCH,
                                                                    input_param)) {
                LOG_AND_TROW_EX(SEU_LERR,
                                script_manager()->getVirtualMachine()->getLastError(),
                                script_manager()->getVirtualMachine()->getLastErrorMessage());
            }
            if(script_manager()->getVirtualMachine()->callProcedure(SEU_ALGORITHM_START,
                                                                    input_param)) {
                LOG_AND_TROW_EX(SEU_LERR,
                                script_manager()->getVirtualMachine()->getLastError(),
                                script_manager()->getVirtualMachine()->getLastErrorMessage());
            }
            break;
    }
    SEU_LAPP << "Script source code update";
    return NULL;
}

void ScriptableExecutionUnit::pluginDirectoryHasBeenUpdated() {

}
