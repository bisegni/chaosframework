/*
 *	AttributeDatasetAction.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/action/AttributeDatasetAction.h>

using namespace chaos::common::utility;
using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::action;

DEFINE_CLASS_FACTORY(AttributeDatasetAction, DataBrokerAction);

AttributeDatasetAction::AttributeDatasetAction(const std::string& name):
DataBrokerAction(name){}

AttributeDatasetAction::~AttributeDatasetAction() {}

void AttributeDatasetAction::executeAction(const AlterDatasetAttributeAction& attribute_dataset_action) {
    switch(attribute_dataset_action.action_type) {
        case AlterDatasetAttributeActionTypeSetValue:
            break;
        case AlterDatasetAttributeActionTypeAddTrigger:
            break;
        case AlterDatasetAttributeActionTypeRemoveTrigger:
            break;
        case AlterDatasetAttributeActionTypeConfigureTrigger:
            break;
        default:
            break;
    }
}
