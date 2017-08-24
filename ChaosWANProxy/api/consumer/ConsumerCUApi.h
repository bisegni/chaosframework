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
#ifndef __CHAOSFramework__ConsumerCUApi__
#define __CHAOSFramework__ConsumerCUApi__

#include "../AbstractApi.h"

namespace chaos {
	namespace wan_proxy {
		namespace api {
			namespace consumer {
				
				class ConsumerCUApi:
				public AbstractApi {
				public:
					//! default constructor
					ConsumerCUApi(persistence::AbstractPersistenceDriver *_persistence_driver);
					
					//! default destructor
					~ConsumerCUApi();
					
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


#endif /* defined(__CHAOSFramework__ConsumerCUApi__) */
