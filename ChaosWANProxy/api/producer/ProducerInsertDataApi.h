/*
 *	ProducerInsertDataApi.h
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
#ifndef __CHAOSFramework__ProducerInsertDataApi__
#define __CHAOSFramework__ProducerInsertDataApi__

#include "../AbstractApi.h"

namespace chaos {
	namespace wan_proxy {
		namespace api {
			namespace producer {
				
				class ProducerInsertDataApi:
				public AbstractApi {
				public:
					//! default constructor
					ProducerInsertDataApi();
					
					//! default destructor
					~ProducerInsertDataApi();
					
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


#endif /* defined(__CHAOSFramework__ProducerInsertDataApi__) */
