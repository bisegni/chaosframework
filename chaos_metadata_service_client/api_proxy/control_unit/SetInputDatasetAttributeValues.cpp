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

#include <chaos_metadata_service_client/api_proxy/control_unit/SetInputDatasetAttributeValues.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

InputDatasetAttributeChangeValue::InputDatasetAttributeChangeValue(const std::string& _attribute_name,
                                                                   const std::string& _value):
attribute_name(_attribute_name),
value(_value){}


ControlUnitInputDatasetChangeSet::ControlUnitInputDatasetChangeSet(const std::string _cu_uid,
                                                                   const std::vector< ChaosSharedPtr<InputDatasetAttributeChangeValue> >& _change_set):
cu_uid(_cu_uid),
change_set(_change_set){}

API_PROXY_CD_DEFINITION(SetInputDatasetAttributeValues,
                        "control_unit",
                        "setInputDatasetAttributeValues")

ApiProxyResult SetInputDatasetAttributeValues::execute(const std::vector< ChaosSharedPtr<ControlUnitInputDatasetChangeSet> >& change_set) {
    CDWUniquePtr message(new chaos::common::data::CDataWrapper());
    
    //compose package
    for(std::vector< ChaosSharedPtr<ControlUnitInputDatasetChangeSet> >::const_iterator it = change_set.begin();
        it != change_set.end();
        it++) {
        //add change for the contorl unit change set
        CDWUniquePtr cu_changes(new CDataWrapper());
        cu_changes->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, (*it)->cu_uid);
        
        for(std::vector< ChaosSharedPtr<InputDatasetAttributeChangeValue> >::iterator it_change = (*it)->change_set.begin();
            it_change != (*it)->change_set.end();
            it_change++) {
            CDWUniquePtr change(new CDataWrapper());
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
    return callApi(message);
}
