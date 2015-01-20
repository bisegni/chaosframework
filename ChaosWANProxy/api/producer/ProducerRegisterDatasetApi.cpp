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

#include <chaos/common/chaos_constants.h>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::producer;

#define PRA_LAPP LAPP_ << "[ProducerRegisterDatasetApi] - "
#define PRA_LDBG LDBG_ << "[ProducerRegisterDatasetApi] - " << __PRETTY_FUNCTION__ << " - "
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
/*!
 
 The name of the producer is given in the uri. The json  need to be composed as follow:
 {
 - the timestamp of the producer got at the registration time
 "ds_timestamp": string,
 
 - the dataset of the producer that is an array of json document
 "ds_desc": [ {
	-the name of the attribute
	"ds_attr_name": string,
 
	-the description fo the attribute
	"ds_attr_desc": string,
 
	-the type of the attribute as in @chaos::DataType::DataType 
	- TYPE_INT32		= 0
	- TYPE_INT64		= 1
	- TYPE_DOUBLE		= 2
	- TYPE_STRING		= 3
	- TYPE_BYTEARRAY	= 4
	- TYPE_BOOLEAN		= 5
	"ds_attr_type": number
 
	- the direction of the attribute o for "input" attribute "output" otherwise.
	
	- output are the attribute the are emitted by producer
	"ds_attr_dir": string,
	
	- the maximum value of the attribute (when applicable)[optional only for input attribute]
	"ds_max_range": string,
	
	- the minimum value of the attribute (when applicable)[optional only for input attribute]
	"ds_min_range": string,
 
	- the default value of the attribute (when applicable)[optional only for input attribute]
	"ds_default_value": "1"
	}]
 }

 */
int ProducerRegisterDatasetApi::execute(std::vector<std::string>& api_tokens,
								 const Json::Value& input_data,
								 std::map<std::string, std::string>& output_header,
								 Json::Value& output_data) {
	int err = 0;
	std::string err_msg;
	if(api_tokens.size() == 0) {
		err_msg = "no producer name in the uri";
		PRA_LERR << err_msg;
		MAKE_API_ERR(output_data, -1, err_msg);
		return err;
	} else if(api_tokens.size() > 1) {
		err_msg = "too many param in the uri";
		PRA_LERR << err_msg;
		
		MAKE_API_ERR(output_data, -2, err_msg);
		return err;
	}
	//we can proceed
	const std::string& producer_name = api_tokens[0];
	PRA_LDBG << "Start registration producer registration with id " << producer_name;

	CDataWrapper mds_registration_pack;
	CDataWrapper dataset_pack;
	//add the producer id
	mds_registration_pack.addStringValue(chaos::DatasetDefinitionkey::DEVICE_ID, producer_name);
	
	//scan the description of the dataset
	const Json::Value& dataset_element = input_data[chaos::DatasetDefinitionkey::DESCRIPTION];
	if(!dataset_element.isArray()) {
		err_msg = "The description of the dataset needs to be an array of object";
		PRA_LERR << err_msg;
		
		MAKE_API_ERR(output_data, -3, err_msg);
		return err;
	}
	
	//we have a dataset, perhaps empty...
	for (Json::ValueConstIterator it = dataset_element.begin();
		 it != dataset_element.end();
		 ++it) {
		if((err = scanDatasetElement(*it, err_msg))) {
			PRA_LERR << err_msg;
			
			MAKE_API_ERR(output_data, err, err_msg);
			return err;
		}
	}
	output_data["register_producer_err"] = 0;
	return err;
}

int ProducerRegisterDatasetApi::scanDatasetElement(const Json::Value& dataset_json_element,
												   std::string& err_msg) {
	return 0;
}