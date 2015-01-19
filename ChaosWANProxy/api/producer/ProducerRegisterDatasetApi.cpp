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
 The json  need to be composed as follow:
 {
 -the name of the producer that we need to register
 "ds_attr_dom": string,
 
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
	Json::FastWriter json_writer;
	
	PRA_LDBG << "Start registration with json pack:";
	
	Json::Value tmp_json_element;
	tmp_json_element = input_data["ds_timestamp"];
	
	output_header.insert(std::make_pair("my header", "my header value"));
	output_data["input_data_received"] = input_data;
	output_data["register_producer_err"] = 0;
	return err;
}