/*
 *	ControlUnitApiInterface.cpp
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

#include <chaos/cu_toolkit/control_manager/ControlUnitApiInterface.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

using namespace chaos::cu::control_manager;

ControlUnitApiInterface::ControlUnitApiInterface(AbstractControlUnit *_control_unit_pointer):
control_unit_pointer(_control_unit_pointer){}

ControlUnitApiInterface::~ControlUnitApiInterface(){}

bool ControlUnitApiInterface::_variantHandler(const std::string& attribute_name,
                                              const chaos::common::data::CDataVariant& value) {
    if(handler_functor == NULL) return true;
    return handler_functor(attribute_name,
                           value);
    
}

void ControlUnitApiInterface::setHandlerFunctor(HandlerFunctor _functor) {
    handler_functor = _functor;
}

bool ControlUnitApiInterface::enableHandlerOnInputAttributeName(const std::string& attribute_name) {
    CHAOS_ASSERT(control_unit_pointer);
    return control_unit_pointer->addVariantHandlerOnInputAttributeName<ControlUnitApiInterface>(this,
                                                                                                &ControlUnitApiInterface::_variantHandler,
                                                                                                attribute_name);
}


bool ControlUnitApiInterface::removeHandlerOnInputAttributeName(const std::string& attribute_name) {
    CHAOS_ASSERT(control_unit_pointer);
    return control_unit_pointer->removeHandlerOnAttributeName(attribute_name);
}
