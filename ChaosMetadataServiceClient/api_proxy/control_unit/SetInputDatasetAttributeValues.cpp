/*
 *	SetInputDatasetAttributeValues.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInputDatasetAttributeValues.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

InputDatasetAttributeChangeValue::InputDatasetAttributeChangeValue(const std::string& _attribute_name,
                                                                   const std::string& _value):
attribute_name(_attribute_name),
value(_value){}


ControlUnitInputDatasetChangeSet::ControlUnitInputDatasetChangeSet(const std::string _cu_uid,
                                                                   const std::vector< boost::shared_ptr<InputDatasetAttributeChangeValue> >& _change_set):
cu_uid(_cu_uid),
change_set(_change_set){}

API_PROXY_CD_DEFINITION(SetInputDatasetAttributeValues,
                        "control_unit",
                        "setInputDatasetAttributeValues")

/*!
 
 */
ApiProxyResult SetInputDatasetAttributeValues::execute(const std::vector< boost::shared_ptr<ControlUnitInputDatasetChangeSet> >& change_set) {
    std::auto_ptr<CDataWrapper> message(new chaos::common::data::CDataWrapper());
    
    //compose package
    for(std::vector< boost::shared_ptr<ControlUnitInputDatasetChangeSet> >::const_iterator it = change_set.begin();
        it != change_set.end();
        it++) {
        //add change for the contorl unit change set
        std::auto_ptr<CDataWrapper> cu_changes(new CDataWrapper());
        cu_changes->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, (*it)->cu_uid);
        
        for(std::vector< boost::shared_ptr<InputDatasetAttributeChangeValue> >::iterator it_change = (*it)->change_set.begin();
            it_change != (*it)->change_set.end();
            it_change++) {
            std::auto_ptr<CDataWrapper> change(new CDataWrapper());
            //wrape the chagne
            change->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, (*it_change)->attribute_name);
            change->addStringValue("change_value", (*it_change)->value);
            
            //append the change to the cu object
            cu_changes->appendCDataWrapperToArray(*change);
        }
        cu_changes->finalizeArrayForKey("change_set");
        
        //add cu changes to the global array
        message->appendCDataWrapperToArray(*cu_changes);
    }
    //close array
    message->finalizeArrayForKey("attribute_set_values");

    //call api
    return callApi(message.release());
}