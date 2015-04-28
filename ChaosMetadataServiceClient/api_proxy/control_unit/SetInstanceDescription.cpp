/*
 *	ControlUnitSetInstanceDescription.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInstanceDescription.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

API_PROXY_CD_DEFINITION(SetInstanceDescription,
                        "control_unit",
                        "setInstanceDescription")


std::auto_ptr<SetInstanceDescriptionHelper> SetInstanceDescription::getNewHelper() {
    return std::auto_ptr<SetInstanceDescriptionHelper>(new SetInstanceDescriptionHelper());
}

/*!

 */
ApiProxyResult SetInstanceDescription::execute(SetInstanceDescriptionHelper& api_data) {
    chaos::common::data::CDataWrapper instance_description;
    chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
        //add the control unit unique id
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, api_data.control_unit_uid);
        // set the type for control unit
    message->addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);



        // add the unit server as parent
    instance_description.addStringValue(chaos::NodeDefinitionKey::NODE_PARENT, api_data.unit_server_uid);

        //add the control unit implementation
    instance_description.addStringValue("control_unit_implementation", api_data.control_unit_implementation);

        //add the control unit implementation
    instance_description.addBoolValue("auto_load", api_data.auto_load);
        // set the load parameter
    instance_description.addStringValue("load_parameter", api_data.load_parameter);

        //add driver description
    if(api_data.driver_descirptions.size()>0) {
        for(CDWListIterator it = api_data.driver_descirptions.begin();
            it != api_data.driver_descirptions.end();
            it++) {
            instance_description.appendCDataWrapperToArray(*it);
        }
        instance_description.finalizeArrayForKey("driver_description");
    }
        //add attribute description
    if(api_data.attribute_value_descriptions.size()>0) {
        for(CDWListIterator it = api_data.attribute_value_descriptions.begin();
            it != api_data.attribute_value_descriptions.end();
            it++) {
            instance_description.appendCDataWrapperToArray(*it);
        }
        instance_description.finalizeArrayForKey("attribute_value_descriptions");
    }

        //add instance description to the message
    message->addCSDataValue("instance_description", instance_description);
    return callApi(message);
}

    //----------------------------------------------------

SetInstanceDescriptionHelper::SetInstanceDescriptionHelper():
control_unit_uid(""),
unit_server_uid(""),
control_unit_implementation(""),
auto_load(false),
load_parameter(""){

}

SetInstanceDescriptionHelper::~SetInstanceDescriptionHelper() {

}

    //!add a new driver description
void SetInstanceDescriptionHelper::addDriverDesscription(const std::string& driver_name,
                                                         const std::string& driver_version,
                                                         const std::string& driver_init_parameter) {
    auto_ptr<CDataWrapper> dd(new CDataWrapper());
    dd->addStringValue("name", driver_name);
    dd->addStringValue("version", driver_version);
    dd->addStringValue("init_parameter", driver_init_parameter);
    driver_descirptions.push_back(dd.release());
}

    //! clear all previously added driver descriptions
void SetInstanceDescriptionHelper::clearAllDriverDescriptions() {
    driver_descirptions.clear();
}

    //! add an attribute range value description for the default value and range
void SetInstanceDescriptionHelper::addAttributeConfig(const std::string& attribute_name,
                                                      const std::string& attribute_default_value,
                                                      const std::string& attribute_max_range,
                                                      const std::string& attribute_min_range) {
    auto_ptr<CDataWrapper> attr(new CDataWrapper());
    attr->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, attribute_name);
    attr->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, attribute_default_value);
    if(attribute_max_range.size()>0)attr->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, attribute_max_range);
    if(attribute_min_range.size()>0)attr->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, attribute_min_range);
    attribute_value_descriptions.push_back(attr.release());

}
    //! remove all previously added attribute range value description
void SetInstanceDescriptionHelper::clearAllAttributeConfig() {
    attribute_value_descriptions.clear();
}