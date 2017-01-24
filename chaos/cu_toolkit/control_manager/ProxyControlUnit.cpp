/*
 *	ProxyControlUnit.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/01/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/ProxyControlUnit.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::exception;
using namespace chaos::common::data::cache;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;

#define PRXCUINFO   INFO_LOG(ProxyControlUnit)
#define PRXCUDBG    DBG_LOG(ProxyControlUnit)
#define PRXCUERR    ERR_LOG(ProxyControlUnit)

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(ProxyControlUnit)

ProxyControlUnit::ProxyControlUnit(const std::string& _control_unit_id):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    "") {
    attribute_value_shared_cache = new AttributeValueSharedCache();
}

ProxyControlUnit::ProxyControlUnit(const std::string& _control_unit_id,
                                   const std::string& _control_unit_param,
                                   const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers) {
    attribute_value_shared_cache = new AttributeValueSharedCache();
}


ProxyControlUnit::~ProxyControlUnit() {
    //release attribute shared cache
    if(attribute_value_shared_cache) {
        delete(attribute_value_shared_cache);
    }
    api_interface_pointer->control_unit_pointer = NULL;
}

boost::shared_ptr<ControlUnitApiInterface> ProxyControlUnit::getProxyApiInterface() {
    if(api_interface_pointer.get() == NULL) {
        api_interface_pointer = boost::shared_ptr<ControlUnitApiInterface>(new ControlUnitApiInterface());
        api_interface_pointer->control_unit_pointer = this;
    }
    return api_interface_pointer;
}

#define GET_OR_RETURN(x)\
if(api_interface_pointer.get() == NULL) return x;

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void ProxyControlUnit::_defineActionAndDataset(CDataWrapper& setup_configuration)  throw(CException) {
    AbstractControlUnit::_defineActionAndDataset(setup_configuration);
    GET_OR_RETURN()
    api_interface_pointer->fireEvent(ControlUnitProxyEventDefine);
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void ProxyControlUnit::init(void *initData) throw(CException) {
    //call parent impl
    AbstractControlUnit::init(initData);
    PRXCUINFO << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::initImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, (void*)NULL, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
    GET_OR_RETURN()
    api_interface_pointer->fireEvent(ControlUnitProxyEventInit);
}

/*!
 Starto the  Control Unit scheduling for device
 */
void ProxyControlUnit::start() throw(CException) {
    //call parent impl
    AbstractControlUnit::start();
    GET_OR_RETURN()
    api_interface_pointer->fireEvent(ControlUnitProxyEventStart);
}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void ProxyControlUnit::stop() throw(CException) {
    //call parent impl
    AbstractControlUnit::stop();
    GET_OR_RETURN()
    api_interface_pointer->fireEvent(ControlUnitProxyEventStop);
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void ProxyControlUnit::deinit() throw(CException) {
    //call parent impl
    AbstractControlUnit::deinit();
    
    PRXCUINFO << "Deinitializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::deinitImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
    GET_OR_RETURN()
    api_interface_pointer->fireEvent(ControlUnitProxyEventDeinit);
}

/*!
 Event for update some CU configuration
 */
CDataWrapper* ProxyControlUnit::updateConfiguration(CDataWrapper* update_pack, bool& detach_param) throw (CException) {
    CDataWrapper *result = AbstractControlUnit::updateConfiguration(update_pack, detach_param);
    std::auto_ptr<CDataWrapper> cu_properties;
    CDataWrapper *cu_property_container = NULL;
    if(update_pack->hasKey(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)){
        cu_property_container = update_pack;
    } else if(update_pack->hasKey("property_abstract_control_unit") &&
              update_pack->isCDataWrapperValue("property_abstract_control_unit")){
        cu_properties.reset(update_pack->getCSDataValue("property_abstract_control_unit"));
        cu_property_container = cu_properties.get();
    }
    return result;
}

void ProxyControlUnit::unitDefineActionAndDataset() throw(CException) {
    
}

void ProxyControlUnit::unitDefineCustomAttribute() {
    
}

void ProxyControlUnit::unitInit() throw(CException) {
    
}

void ProxyControlUnit::unitStart() throw(CException) {
    
}

void ProxyControlUnit::unitStop() throw(CException) {
    
}

void ProxyControlUnit::unitDeinit() throw(CException) {
    
}
