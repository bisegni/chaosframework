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

#include <chaos/common/chaos_constants.h>

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
	CDataWrapper output_dataset;
	
	const Json::Value& dataset_timestamp = input_data[chaos::DataPackCommonKey::DPCK_TIMESTAMP];
	if(dataset_timestamp.isNull()) {
		err_msg = "The timestamp is mandatory";
		PID_LERR << err_msg;
		PRODUCER_INSERT_ERR(output_data, -3, err_msg);
		return err;
	}else if(!dataset_timestamp.isNumeric()) {
		err_msg = "The timestamp needs to be an int64 number";
		PID_LERR << err_msg;
		PRODUCER_INSERT_ERR(output_data, -4, err_msg);
		return err;
	}
	output_dataset.addInt64Value(chaos::DataPackCommonKey::DPCK_TIMESTAMP, dataset_timestamp.asInt64());
	
	output_data["register_producer_err"] = 0;
	return err;
}