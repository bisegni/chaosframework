/*
 *	ProducerInsertDatasetApi.cpp
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
#include "ProducerInsertDatasetApi.h"

#include <cstring>

#include <chaos/common/chaos_constants.h>

#include <boost/algorithm/string.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::producer;

#define PRODUCER_INSERT_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_insert_err", err, "producer_insert_err_msg", msg)

#define PID_LAPP LAPP_ << "[ProducerInsertDatasetApi] - "
#define PID_LDBG LDBG_ << "[ProducerInsertDatasetApi] - "
#define PID_LERR LERR_ << "[ProducerInsertDatasetApi] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

//! default constructor
ProducerInsertDatasetApi::ProducerInsertDatasetApi(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("insert",
			_persistence_driver){
	
}

//! default destructor
ProducerInsertDatasetApi::~ProducerInsertDatasetApi() {
	
}

//! execute the api
int ProducerInsertDatasetApi::execute(std::vector<std::string>& api_tokens,
									  const Json::Value& input_data,
									  std::map<std::string, std::string>& output_header,
									  Json::Value& output_data) {
	CHAOS_ASSERT(persistence_driver)
	int err = 0;
	std::string err_msg;
	std::vector<std::string> kv_splitted;
	
	if(api_tokens.size() == 0) {
		err_msg = "no producer name in the uri";
		PID_LERR << err_msg;
		PRODUCER_INSERT_ERR(output_data, -1, err_msg);
		return err;
	} else if(api_tokens.size() > 1) {
		err_msg = "too many param in the uri";
		PID_LERR << err_msg;
		
		PRODUCER_INSERT_ERR(output_data, -2, err_msg);
		return err;
	}
	
	if(input_data[chaos::DataPackCommonKey::DPCK_TIMESTAMP].isNull()) {
		err_msg = "The timestamp is mandatory";
		PID_LERR << err_msg;
		PRODUCER_INSERT_ERR(output_data, -3, err_msg);
		return err;
	} else if(!input_data[chaos::DataPackCommonKey::DPCK_TIMESTAMP].isString()) {
	  err_msg = "The timestamp needs to be an int64 number," + input_data[chaos::DataPackCommonKey::DPCK_TIMESTAMP].asString();
	  PID_LERR << err_msg;
	  PRODUCER_INSERT_ERR(output_data, -4, err_msg);
	  return err;
	}
    
	//we can proceed
	auto_ptr<CDataWrapper> output_dataset(new CDataWrapper());
	const std::string& producer_name = api_tokens[0];


	Json::Value::Members members = input_data.getMemberNames();
	for(Json::Value::Members::iterator it = members.begin();
		it != members.end();
		it++) {
		//get current value
		const Json::Value& dataset_element = input_data[*it];
		
		//check for correctness of the value
		if(dataset_element.isNull()) {
			err_msg = "The dataset attribute cant be null";
			PID_LERR << err_msg;
			PRODUCER_INSERT_ERR(output_data, -4, err_msg);
			return err;
		}else if(!dataset_element.isString()) {
		  err_msg = "The dataset element needs to be only string:"+dataset_element.asString();
			PID_LERR << err_msg;
			PRODUCER_INSERT_ERR(output_data, -5, err_msg);
			return err;
		}

		//split the value
		const std::string& attribute_value = dataset_element.asString();
		boost::algorithm::split(kv_splitted,
								attribute_value,
								boost::algorithm::is_any_of(":"),
								boost::algorithm::token_compress_on);
		
		//chec if the value is composed by two token (type and value)
		if(kv_splitted.size()!=2) {
			err_msg = "invalid value of the dataset attribute " + *it;
			PID_LERR << err_msg;
			PRODUCER_INSERT_ERR(output_data, -5, err_msg);
			return err;
		}
		
		if((err = setValueFromString(*output_dataset,
						   kv_splitted[0],
						   *it,
						   kv_splitted[1]))) {
			//there is an error
			err_msg = "Error during the setting of the value ["+ attribute_value +"] for the attribute ["+ *it +"]";
			PID_LERR << err_msg;
			PRODUCER_INSERT_ERR(output_data, err, err_msg);
			return err;

		}
		//clear for next check
		kv_splitted.clear();
	}
	
	//call persistence api for insert the data
	if((err = persistence_driver->pushNewDataset(producer_name,
												 output_dataset.get(),
												 2))) {
		err_msg = "Error during push of the dataset";
		PID_LERR << err_msg;
		PRODUCER_INSERT_ERR(output_data, err, err_msg);
		return err;
	} else {
		output_dataset.release();
	}
	
	output_data["register_insert_err"] = 0;
	return err;
}
