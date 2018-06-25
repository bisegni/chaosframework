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

#include <chaos/common/script/ScriptManager.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/script/AbstractScriptableClass.h>

using namespace chaos::common::script;
using namespace chaos::common::utility;

#define SCRTMAN_INFO   INFO_LOG(ScriptManager)
#define SCRPTMAN_DBG   INFO_LOG(ScriptManager)
#define SCRPTMAN_ERR    ERR_LOG(ScriptManager)

ScriptManager::ScriptManager(const std::string& _script_language):
vm_name(CHAOS_FORMAT("%1%ScriptVM", %_script_language)),
script_language(_script_language),
script_vm(NULL){}

ScriptManager::~ScriptManager() {
    map_api_class.clear();
}

void ScriptManager::registerApiClass(AbstractScriptableClass& api_class) {
        //write lock the map
    boost::unique_lock<boost::shared_mutex> rl(map_mutex);
    if(map_api_class.count(api_class.api_class_name)) return;
        //add api class
    SCRPTMAN_DBG << CHAOS_FORMAT("Register api class for %1% class", %api_class.api_class_name);
    
    api_class.script_manager_ptr = this;
    map_api_class.insert(make_pair(api_class.api_class_name, &api_class));
}

void ScriptManager::deregisterApiClass(AbstractScriptableClass& api_class) {
        //write lock on map
    boost::unique_lock<boost::shared_mutex> rl(map_mutex);
    if(map_api_class.count(api_class.api_class_name) == 0) return;
        //remove api class
    SCRPTMAN_DBG << CHAOS_FORMAT("Deregister api class for %1% class", %api_class.api_class_name);
    
    api_class.script_manager_ptr = NULL;
    map_api_class.erase(api_class.api_class_name);
}

    //!entry point of the scirpting for call an exposed api
int ScriptManager::callScriptApi(const std::string& api_class,
                                 const std::string& api_name,
                                 const ScriptInParam& input_parameter,
                                 ScriptOutParam& output_parameter) {
        //read lock the map
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
    if(map_api_class.count(api_class) == 0) {
        throw CException(-1, CHAOS_FORMAT("Api class %1% is not registered", %api_class), __PRETTY_FUNCTION__);
    };
        //call api class
    return map_api_class[api_class]->callApi(api_name,
                                             input_parameter,
                                             output_parameter);
}

void ScriptManager::init(void *init_data) throw(chaos::CException) {
    SCRTMAN_INFO << "Use " << script_language << " as script language";
    SCRTMAN_INFO << "Try to load " << vm_name << " virtual machine";
    script_vm = ObjectFactoryRegister<AbstractScriptVM>::getInstance()->getNewInstanceByName(vm_name);
    CHAOS_LASSERT_EXCEPTION(script_vm, SCRPTMAN_ERR, -1, CHAOS_FORMAT("Virtual machine '%1%' has not been instantiated", %vm_name));
    script_vm->setCaller(this);
    InizializableService::initImplementation(script_vm, NULL, vm_name, __PRETTY_FUNCTION__);
}

void ScriptManager::deinit() throw(chaos::CException) {
    if(script_vm) {
        InizializableService::deinitImplementation(script_vm, vm_name, __PRETTY_FUNCTION__);
        delete(script_vm);
        script_vm = NULL;
    }
}

AbstractScriptVM *ScriptManager::getVirtualMachine() {
    return script_vm;
}
