/*
 *	ProducerRegisterAPI.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "ProducerRegisterDatasetApi.h"

#include <algorithm>
#include <string>

#include <chaos/common/chaos_constants.h>

#include <boost/lexical_cast.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::producer;

#define PRODUCER_REGISTER_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_register_err", err, "producer_register_err_msg", msg)

#define PRA_LAPP LAPP_ << "[ProducerRegisterDatasetApi] - "
#define PRA_LDBG LDBG_ << "[ProducerRegisterDatasetApi] - "
#define PRA_LERR LERR_ << "[ProducerRegisterDatasetApi] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

//! default constructor
ProducerRegisterDatasetApi::ProducerRegisterDatasetApi(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("register",
			_persistence_driver){
	
}

//! default destructor
ProducerRegisterDatasetApi::~ProducerRegisterDatasetApi() {
	
}

//! execute the api
int ProducerRegisterDatasetApi::execute(std::vector<std::string>& api_tokens,
										const Json::Value& input_data,
										std::map<std::string, std::string>& output_header,
										Json::Value& output_data) {
	CHAOS_ASSERT(persistence_driver)
	int err = 0;
	std::string err_msg;
	std::string producer_name;
	int cnt;
	if(api_tokens.size() == 0) {
		err_msg = "no producer name in the uri";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -1, err_msg);
		return err;
	} /*else if(api_tokens.size() > 1) {
		err_msg = "too many param in the uri";
		PRA_LERR << api_tokens.size()<<" producer:"<<api_tokens[0]<<" other:"<<api_tokens[1]<<err_msg;
		
		PRODUCER_REGISTER_ERR(output_data, -2, err_msg);
		return err;
		}*/

	for(cnt = 0;cnt<api_tokens.size();cnt++){
	  
	  if(cnt<api_tokens.size()-1){
	    producer_name=producer_name + api_tokens[cnt] + "/";
	  } else {
	    producer_name=producer_name + api_tokens[cnt] ;
	  }
	}

	 

	//we can proceed
	//const std::string& producer_name = api_tokens[0];
	PRA_LDBG << "Start producer registration with id " << producer_name;
	
	CDataWrapper mds_registration_pack;
	CDataWrapper dataset_pack;
	
	const Json::Value& dataset_timestamp = input_data[chaos::DatasetDefinitionkey::TIMESTAMP];
	if(dataset_timestamp.isNull()) {
		err_msg = "The timestamp is mandatory";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -3, err_msg);
		return err;
	} else if(!dataset_timestamp.isInt64()) {
		err_msg = "The timestamp needs to be an int64 number";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -4, err_msg);
		return err;
	}
	mds_registration_pack.addInt64Value(chaos::DatasetDefinitionkey::TIMESTAMP,
										(int64_t)dataset_timestamp.asUInt64());
	
	//scan the description of the dataset
	const Json::Value& dataset_elements = input_data[chaos::DatasetDefinitionkey::DESCRIPTION];
	if(!dataset_elements.isArray()) {
		err_msg = "The description of the dataset needs to be an array of object";
		PRA_LERR << err_msg;
		
		PRODUCER_REGISTER_ERR(output_data, -5, err_msg);
		return err;
	}
	//we have a dataset, perhaps empty...
	for (Json::ValueConstIterator it = dataset_elements.begin();
		 it != dataset_elements.end();
		 ++it) {
		boost::shared_ptr<CDataWrapper> element;
		if((err = scanDatasetElement(*it, err_msg, element))) {
			PRA_LERR << err_msg;
			
			PRODUCER_REGISTER_ERR(output_data, err, err_msg);
			return err;
		} else {
			// add parametere representation object to main action representation
			mds_registration_pack.appendCDataWrapperToArray(*element.get());
		}
	}
	
	//close array for all device description
	mds_registration_pack.finalizeArrayForKey(DatasetDefinitionkey::DESCRIPTION);
	DEBUG_CODE(PRA_LDBG << mds_registration_pack.getJSONString());
	
	if((err = persistence_driver->registerDataset(producer_name,
												  mds_registration_pack))) {
		err_msg = "Error in the dataset registration";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -18, err_msg);
		return err;
	}else {
		output_data["producer_register_err"] = 0;
		output_data["producer_register_bson"] =  mds_registration_pack.getJSONString();
	}
	return err;
}

//scan a json elemenot of the dataset creating the respective CDataWrapper
int ProducerRegisterDatasetApi::scanDatasetElement(const Json::Value& dataset_json_element,
												   std::string& err_msg,
												   boost::shared_ptr<chaos::common::data::CDataWrapper>& element) {
	element.reset(new CDataWrapper());
	/*-the name of the attribute
	 "ds_attr_name": string,
	 */
	const Json::Value&  attribute_name = dataset_json_element[chaos::DatasetDefinitionkey::ATTRIBUTE_NAME];
	if(attribute_name.isNull()) {
		err_msg = "no attribute name found";
		return -6;
	} else if(!attribute_name.isString()) {
		err_msg = "the name of the attribute needs to be a string";
		return -7;
	}
	
	element->addStringValue(chaos::DatasetDefinitionkey::ATTRIBUTE_NAME,
							attribute_name.asString());
	/*
	 -the description fo the attribute
	 "ds_attr_desc": string,
	 */
	const Json::Value&  attribute_description = dataset_json_element[chaos::DatasetDefinitionkey::ATTRIBUTE_DESCRIPTION];
	if(attribute_description.isNull()) {
		err_msg = "no attribute description found";
		return -8;
	} else if(!attribute_description.isString()) {
		err_msg = "the description of the attribute needs to be a string";
		return -9;
	}
	element->addStringValue(chaos::DatasetDefinitionkey::ATTRIBUTE_DESCRIPTION,
							attribute_description.asString());
	/*
	 -the type of the attribute as in @chaos::DataType::DataType
	 - TYPE_INT32		= 0
	 - TYPE_INT64		= 1
	 - TYPE_DOUBLE		= 2
	 - TYPE_STRING		= 3
	 - TYPE_BYTEARRAY	= 4
	 - TYPE_BOOLEAN		= 5
	 "ds_attr_type": number
	 */
	const Json::Value&  attribute_type = dataset_json_element[chaos::DatasetDefinitionkey::ATTRIBUTE_TYPE];
	if(attribute_type.isNull()) {
		err_msg = "the attribute type is mandatory";
		return -10;
	} else if(!attribute_type.isString()) {
		err_msg = "the attribute type needs to be a string";
		return -11;
	}
	std::string type = attribute_type.asString();
	std::transform(type.begin(), type.end(), type.begin(), ::tolower);
	
	if(type.compare("int32") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_INT32);
	}else if(type.compare("int64") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_INT64);
	}else if(type.compare("double") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_DOUBLE);
	}else if(type.compare("string") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_STRING);
	}else if(type.compare("binary") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_BYTEARRAY);
	}else if(type.compare("boolean") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::TYPE_BOOLEAN);
	}else{
		err_msg = "Bad attribute type";
		return -12;
	}
	
	/*
	 - the direction of the attribute o for "input" attribute "output" otherwise.
		
		- output are the attribute the are emitted by producer
		"ds_attr_dir": string,
		
	 */
	const Json::Value&  attribute_direction = dataset_json_element[chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION];
	if(attribute_direction.isNull()) {
		err_msg = "the attribute direction is mandatory";
		return -13;
	} else if(!attribute_direction.isString()) {
		err_msg = "the attribute direction needs to be a string";
		return -14;
	}
	
	std::string direction = attribute_direction.asString();
	std::transform(direction.begin(), direction.end(), direction.begin(), ::tolower);
	
	if(direction.compare("input") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::Input);
	} else if(direction.compare("output") == 0) {
		element->addInt32Value(chaos::DatasetDefinitionkey::ATTRIBUTE_DIRECTION,
							   chaos::DataType::Output);
	}
	
	/*
		- the maximum value of the attribute (when applicable)[optional only for input attribute]
		"ds_max_range": string,
		*/
	const Json::Value&  attribute_max_range = dataset_json_element[chaos::DatasetDefinitionkey::MAX_RANGE];
	if(!attribute_max_range.isNull()) {
		if(!attribute_max_range.isString()) {
			err_msg = "the attribute max value range needs to be a string";
			return -15;
		}
		element->addStringValue(chaos::DatasetDefinitionkey::MAX_RANGE,
								attribute_max_range.asString());
	}
	/*
		- the minimum value of the attribute (when applicable)[optional only for input attribute]
		"ds_min_range": string,
		*/
	const Json::Value&  attribute_min_range = dataset_json_element[chaos::DatasetDefinitionkey::MIN_RANGE];
	if(!attribute_min_range.isNull()) {
		if(!attribute_min_range.isString()) {
			err_msg = "the attribute max value range needs to be a string";
			return -16;
		}
		element->addStringValue(chaos::DatasetDefinitionkey::MIN_RANGE,
								attribute_min_range.asString());
	}
	/*
		- the default value of the attribute (when applicable)[optional only for input attribute]
		"ds_default_value": "1"
	 */
	const Json::Value&  attribute_default_range = dataset_json_element[chaos::DatasetDefinitionkey::DEFAULT_VALUE];
	if(!attribute_default_range.isNull()) {
		if(!attribute_default_range.isString()) {
			err_msg = "the attribute max value range needs to be a string";
			return -17;
		}
		element->addStringValue(chaos::DatasetDefinitionkey::DEFAULT_VALUE,
								attribute_default_range.asString());
	}
	return 0;
}
