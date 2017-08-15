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
#ifndef __CHAOSFramework__ProducerGetLastDataApi__
#define __CHAOSFramework__ProducerGetLastDataApi__

#include "../AbstractApi.h"

namespace chaos {
	namespace wan_proxy {
		namespace api {
			namespace producer {
				
				class ProducerGetLastDatasetApi:
				public AbstractApi {
				public:
					//! default constructor
					ProducerGetLastDatasetApi(persistence::AbstractPersistenceDriver *_persistence_driver);
					
					//! default destructor
					~ProducerGetLastDatasetApi();
					
					//! execute the api
					int execute(std::vector<std::string>& api_tokens,
								const Json::Value& input_data,
								std::map<std::string, std::string>& output_header,
								Json::Value& output_data);
				};
				
			}
		}
	}
}


#endif /* defined(__CHAOSFramework__ProducerGetLastDatasetApi__) */
