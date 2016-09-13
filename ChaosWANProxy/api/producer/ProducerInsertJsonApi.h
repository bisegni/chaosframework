/*
 *	ProducerInsertJsonApi.h
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
#ifndef __CHAOSFramework__ProducerInsertJsonApi__
#define __CHAOSFramework__ProducerInsertJsonApi__

#include "../AbstractApi.h"

namespace chaos {
	namespace wan_proxy {
		namespace api {
			namespace producer {
				
				class ProducerInsertJsonApi:
				public AbstractApi {
				public:
					//! default constructor
					ProducerInsertJsonApi(persistence::AbstractPersistenceDriver *_persistence_driver);
					
					//! default destructor
					~ProducerInsertJsonApi();
					
					//! execute the api
					/*!
					 
					 The name of the producer is given in the uri. The json  need to be composed as follow:
					 {
					 - the timestamp of the producer got at the registration time
					 "dpck_ts": "uint64:value",
					 
					 - the key of the output attribute and is value
					 "attribute_name":"type:attribute_value"
					 type neds to be a value of: int32, int64, double, string, binary, boolean
					 
					 the binary needs to be sent as base64 encoded string
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


#endif /* defined(__CHAOSFramework__ProducerInsertJsonApi__) */
