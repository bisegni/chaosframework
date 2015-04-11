/*
 *	AbstractApiGroup.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__AbstractApiGroup__
#define __CHAOSFramework__AbstractApiGroup__

#include "AbstractApi.h"
#include "PersistenceAccessor.h"

#include "../utility/TypedConstrainedHashMap.h"

#include <string>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

#include <json/json.h>

namespace chaos {
	namespace wan_proxy {
		namespace api {
			
			typedef utility::TypedConstrainedHashMap<AbstractApi> ApiHashMap;
			
			//! define the abstract group of api
			class AbstractApiGroup:
			public PersistenceAccessor,
			public chaos::common::utility::NamedService,
			protected ApiHashMap {
				
				//! inherited from AbstractApiHashMap
				void clearHashTableElement(const void *key,
										   uint32_t key_len,
										   AbstractApi *element);
			protected:
				template<typename A>
				void addApi() {
					addTypeWithParam<A, persistence::AbstractPersistenceDriver*>(getPersistenceDriver());
				}
			public:
				//! construct the group with a name
				AbstractApiGroup(const std::string& name,
								 persistence::AbstractPersistenceDriver *_persistence_driver);
				
				//! default destructor
				virtual ~AbstractApiGroup();
				
				//! remove all api
				void removeAllApi();
				
				//! remove by name
				void removeApiByName(const std::string name);
				
				int callApi(std::vector<std::string>& api_tokens,
							const Json::Value& input_data,
							std::map<std::string, std::string>& output_header,
							Json::Value& output_data);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractApiGroup__) */
