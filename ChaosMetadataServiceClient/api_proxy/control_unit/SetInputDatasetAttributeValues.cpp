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

InputDatasetAttributeValue::InputDatasetAttributeValue(const std::string& _cu_uid,
                                                       const std::string& _attribute_name,
                                                       const std::string& _value):
cu_uid(_cu_uid),
attribute_name(_attribute_name),
value(_value){}

API_PROXY_CD_DEFINITION(SetInputDatasetAttributeValues,
                        "control_unit",
                        "setInputDatasetAttributeValues")

/*!
 
 */
ApiProxyResult SetInputDatasetAttributeValues::execute(const std::vector< boost::shared_ptr<InputDatasetAttributeValue> >& attribute_values) {
    std::auto_ptr<CDataWrapper> message(new chaos::common::data::CDataWrapper());
    
    //compose package
    for(std::vector< boost::shared_ptr<InputDatasetAttributeValue> >::const_iterator it = attribute_values.begin();
        it != attribute_values.end();
        it++) {
        
        std::auto_ptr<CDataWrapper> element;
        element->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, (*it)->cu_uid);
        element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, (*it)->attribute_name);
        element->addStringValue("set_value", (*it)->value);
        
        message->appendCDataWrapperToArray(*element);
    }
    //close array
    message->finalizeArrayForKey("attribute_set_values");
    //call api
    return callApi(message.release());
}