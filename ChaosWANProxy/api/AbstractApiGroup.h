/*
 *	AbstractApiGroup.h
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
#ifndef __CHAOSFramework__AbstractApiGroup__
#define __CHAOSFramework__AbstractApiGroup__

#include "AbstractApi.h"

#include <string>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

namespace chaos {
	namespace wan_proxy {
		namespace api {
			
			typedef chaos::common::utility::TemplatedKeyValueHashMap<AbstractApi*> AbstractApiHashMap;
			
			//! define the abstract group of api
			class AbstractApiGroup:
			public chaos::common::utility::NamedService,
			private AbstractApiHashMap {
				
				//! inherited from AbstractApiHashMap
				void clearHashTableElement(const void *key,
										   uint32_t key_len,
										   AbstractApi *element);
			public:
				//! construct the group with a name
				AbstractApiGroup(const std::string& name);
				
				//! default destructor
				virtual ~AbstractApiGroup();
				
				//! add a class instance as api with a name
				template <typename A>
				void addApi(const std::string& name) {
					INSTANCER(A, AbstractApi) i = ALLOCATE_INSTANCER(A, AbstractApi);
					addElement(name.c_str(),
							   name.size(),
							   i.getInstance());
				}
				
				//! add a class instance as api the name is got from the service name
				template <typename A>
				void addApi() {
					INSTANCER(A, AbstractApi) i = ALLOCATE_INSTANCER(A, AbstractApi);
					addElement(i.getName().c_str(),
							   i.getName().c_str(),
							   i.getInstance());
				}
				
				//! remove all api
				void removeAllApi();
				
				//! remove by name
				void removeApiByName(const std::string name);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractApiGroup__) */
