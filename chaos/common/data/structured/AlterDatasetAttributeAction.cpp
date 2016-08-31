/*
 *	DatabrokerAlterDSAttribute.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 30/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/structured/AlterDatasetAttributeAction.h>

using namespace chaos::common::data::structured;

#pragma mark AlterDatasetAttributeSetValue
AlterDatasetAttributeSetValue::AlterDatasetAttributeSetValue():
dataset_name(),
attribute_name(){}

AlterDatasetAttributeSetValue::AlterDatasetAttributeSetValue(const AlterDatasetAttributeSetValue& src):
dataset_name(src.dataset_name),
attribute_name(src.attribute_name){}


#pragma mark AlterDatasetAttributeAddTrigger
AlterDatasetAttributeAddTrigger::AlterDatasetAttributeAddTrigger():
dataset_name(),
attribute_name(),
event_name(),
trigger_name(){}

AlterDatasetAttributeAddTrigger::AlterDatasetAttributeAddTrigger(const AlterDatasetAttributeAddTrigger& src):
dataset_name(src.dataset_name),
attribute_name(src.attribute_name),
event_name(src.event_name),
trigger_name(src.trigger_name){}


#pragma mark AlterDatasetAttributeRemoveTrigger
AlterDatasetAttributeRemoveTrigger::AlterDatasetAttributeRemoveTrigger():
dataset_name(),
attribute_name(),
event_name(),
trigger_name_uid(){}

AlterDatasetAttributeRemoveTrigger::AlterDatasetAttributeRemoveTrigger(const AlterDatasetAttributeRemoveTrigger& src):
dataset_name(src.dataset_name),
attribute_name(src.attribute_name),
event_name(src.event_name),
trigger_name_uid(src.trigger_name_uid){}

#pragma mark AlterDatasetAttributeConfigureTrigger
AlterDatasetAttributeConfigureTrigger::AlterDatasetAttributeConfigureTrigger():
dataset_name(),
attribute_name(),
event_name(),
trigger_name_uid(),
trigger_properties(){}

AlterDatasetAttributeConfigureTrigger::AlterDatasetAttributeConfigureTrigger(const AlterDatasetAttributeConfigureTrigger& src):
dataset_name(src.dataset_name),
attribute_name(src.attribute_name),
event_name(src.event_name),
trigger_name_uid(src.trigger_name_uid),
trigger_properties(src.trigger_properties){}

#pragma mark AlterDatasetAttributeAction
AlterDatasetAttributeSetValue&  AlterDatasetAttributeAction::getSetValueAction() {
    return boost::get<AlterDatasetAttributeSetValue>(action_description);
}

AlterDatasetAttributeAddTrigger& AlterDatasetAttributeAction::getAddTriggerAction() {
    return boost::get<AlterDatasetAttributeAddTrigger>(action_description);
}

AlterDatasetAttributeRemoveTrigger& AlterDatasetAttributeAction::getRemoveTriggerAction() {
    return boost::get<AlterDatasetAttributeRemoveTrigger>(action_description);
}

AlterDatasetAttributeConfigureTrigger& AlterDatasetAttributeAction::getConfigureTriggerAction()  {
    return boost::get<AlterDatasetAttributeConfigureTrigger>(action_description);
}
