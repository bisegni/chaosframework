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


using namespace chaos::wan_proxy::api::producer;

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
	int err = 0;
	
	for(std::vector<std::string>::iterator it = api_tokens.begin();
		it != api_tokens.end();
		it++) {
		output_data["register_producer_input"].append(*it);
	}
	
	output_header.insert(std::make_pair("my header", "my header value"));
	output_data["register_producer_err"] = 0;
	return err;
}