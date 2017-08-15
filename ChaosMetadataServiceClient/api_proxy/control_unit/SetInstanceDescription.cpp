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

#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInstanceDescription.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

API_PROXY_CD_DEFINITION(SetInstanceDescription,
                        "control_unit",
                        "setInstanceDescription")


ChaosUniquePtr<SetInstanceDescriptionHelper> SetInstanceDescription::getNewHelper() {
    return ChaosUniquePtr<SetInstanceDescriptionHelper>(new SetInstanceDescriptionHelper());
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
        //add the auto load setting
    instance_description.addBoolValue("auto_load", api_data.auto_load);
        //add the auto init setting
    instance_description.addBoolValue("auto_init", api_data.auto_init);
        //add the auto start setting
    instance_description.addBoolValue("auto_start", api_data.auto_start);
    	// add the description field
    instance_description.addStringValue("desc", api_data.desc);
        // set the load parameter
    instance_description.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_LOAD_PARAM, api_data.load_parameter);
        //add the deafult scheduler delay
    instance_description.addInt64Value(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, api_data.default_schedule_delay);
    //add the storage type
    instance_description.addInt32Value(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, api_data.storage_type);
    //add the ageing
    instance_description.addInt32Value(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING, api_data.history_ageing);
    //add the history rate
    instance_description.addInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, api_data.history_time);
    //add the live rate
    instance_description.addInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME, api_data.live_time);
        //add driver description
    if(api_data.driver_descriptions.size()>0) {
        for(CDWListIterator it = api_data.driver_descriptions.begin();
            it != api_data.driver_descriptions.end();
            it++) {
            instance_description.appendCDataWrapperToArray(*it);
        }
        instance_description.finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION);
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

    instance_description.addCSDataValue(NodeDefinitionKey::NODE_CUSTOM_PARAM,api_data.control_unit_custom_param);


        //add instance description to the message
    message->addCSDataValue("instance_description", instance_description);
    return callApi(message);
}

ApiProxyResult SetInstanceDescription::execute(const std::string& uid,chaos::common::data::CDataWrapper& instance_description){
	 chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
	        //add the control unit unique id
	message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, uid);
	        // set the type for control unit
	message->addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);


	message->addCSDataValue("instance_description", instance_description);
	return callApi(message);
}


    //----------------------------------------------------

SetInstanceDescriptionHelper::SetInstanceDescriptionHelper():
control_unit_uid(""),
unit_server_uid(""),
desc("Control Unit"),
control_unit_implementation(""),
auto_load(false),
auto_init(false),
auto_start(false),
storage_type(DataServiceNodeDefinitionType::DSStorageTypeLive),
history_ageing(0),
history_time(0),
live_time(0),
default_schedule_delay(1000000),
load_parameter(""){}

SetInstanceDescriptionHelper::~SetInstanceDescriptionHelper() {}

    //!add a new driver description
void SetInstanceDescriptionHelper::addDriverDescription(const std::string& driver_name,
                                                        const std::string& driver_version,
                                                        const std::string& driver_init_parameter) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> dd(new CDataWrapper());
    dd->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_NAME, driver_name);
    dd->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_VERSION, driver_version);
    dd->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_INIT_PARAMETER, driver_init_parameter);
    driver_descriptions.push_back(dd.release());
}

    //! clear all previously added driver descriptions
void SetInstanceDescriptionHelper::clearAllDriverDescriptions() {
    driver_descriptions.clear();
}

    //! add an attribute range value description for the default value and range
void SetInstanceDescriptionHelper::addAttributeConfig(const std::string& attribute_name,
                                                      const std::string& attribute_default_value,
                                                      const std::string& attribute_max_range,
                                                      const std::string& attribute_min_range) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> attr(new CDataWrapper());
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
