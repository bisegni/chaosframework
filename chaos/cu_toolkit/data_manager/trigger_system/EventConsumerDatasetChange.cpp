/*
 *	EventConsumerDatasetChange.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/EventConsumerDatasetChange.h>

using namespace chaos::common::data;
using namespace chaos::cu::data_manager::trigger_system;

#pragma mark TriggerEventSetDSAttribute
TriggerEventSetDSAttribute::TriggerEventSetDSAttribute(const ETDatasetAttributeChangePhase _phase):
TriggerEvent("TriggerEventPreSetDSAttribute", 0),
phase(_phase){}



#pragma mark EventConsumerSetDSAttribute
EventConsumerSetDSAttribute::EventConsumerSetDSAttribute():
AbstractEventConsumer("EventConsumerSetDSAttribute"){}

EventConsumerSetDSAttribute::~EventConsumerSetDSAttribute(){}

#pragma smark EventConsumerSetDSAttributeMaxMin

ETDatasetAttributeChangeResult EventConsumerSetDSAttributeMaxMin::executePre(ETDatasetAttributeChangePhase event_type,
                                                                             CDataVariant attribute_new_value,
                                                                             CDataVariant attribute_converted_value) {
    return ETDatasetAttributeChangeResultOK;
}

ETDatasetAttributeChangeResult EventConsumerSetDSAttributeMaxMin::executePost(ETDatasetAttributeChangePhase event_type,
                                                                              CDataVariant attribute_values) {
    return ETDatasetAttributeChangeResultOK;
}
