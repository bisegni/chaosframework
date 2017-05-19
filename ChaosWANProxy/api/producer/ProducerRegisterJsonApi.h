/*
 *	ProducerRegisterJsonApi.h
 *	!CHAOS
 *	Created by Andrea Michelotti.
 *
 *    	Copyrigh 2016 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__ProducerRegisterJsonAPI__
#define __CHAOSFramework__ProducerRegisterJsonAPI__

#include "../AbstractApi.h"

#include <boost/shared_ptr.hpp>
namespace chaos {
	namespace wan_proxy {
		namespace api {
			namespace producer {
				
				typedef enum ProducerRegisterJsonApiErrorCode {
					
				}ProducerRegisterJsonApiErrorCode;
				
				class ProducerRegisterJsonApi:
				public AbstractApi {

				public:
					//! default constructor
					ProducerRegisterJsonApi(persistence::AbstractPersistenceDriver *_persistence_driver);
					
					//! default destructor
					~ProducerRegisterJsonApi();
					
					//! execute the api
					/*!
					 The name of the producer is given in the uri. The json  need to be composed as follow:
					 {
					 - the timestamp of the producer got at the registration time
					 "ds_timestamp": number,
					 
					 - the dataset of the producer that is an array of json document
					 "ds_desc": [ {
					 -the name of the attribute
					 "ds_attr_name": string,
					 
					 -the description fo the attribute
					 "ds_attr_desc": string,
					 
					 -the type of the attribute as: int32, int64, double, string, binary, boolean
					 "ds_attr_type": string
					 
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
					int execute(std::vector<std::string>& api_tokens,
								const Json::Value& input_data,
								std::map<std::string, std::string>& output_header,
								Json::Value& output_data);
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__ProducerRegisterJsonApi__) */
