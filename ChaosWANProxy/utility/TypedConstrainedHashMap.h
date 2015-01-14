/*
 *	TypedConstrainedHashMap.h
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
#ifndef CHAOSFramework_TypedConstrainedHashMap_h
#define CHAOSFramework_TypedConstrainedHashMap_h

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

namespace chaos {
	namespace wan_proxy {
		namespace utility {
			
			//! define the abstract typede hash map
			template<typename T>
			class TypedConstrainedHashMap:
			protected chaos::common::utility::TemplatedKeyValueHashMap<T> {
				
			public:
				//! construct the group with a name
				TypedConstrainedHashMap(uint32_t hash_power):common::utility::TemplatedKeyValueHashMap<T>(hash_power){};
				
				//! default destructor
				virtual ~TypedConstrainedHashMap(){};
				
				//! add a class instance as api with a name
				template <typename A>
				void addApi(const std::string& name) {
					INSTANCER(A, T) i = ALLOCATE_INSTANCER(A, T);
					addElement(name.c_str(),
							   name.size(),
							   i.getInstance());
				}
				
				//! add a class instance as api the name is got from the service name
				template <typename A>
				void addApi() {
					INSTANCER(A, T) i = ALLOCATE_INSTANCER(A, T);
					addElement(i.getName().c_str(),
							   i.getName().c_str(),
							   i.getInstance());
				}
				
				void removeAll() {
					//call hash map clear method
					chaos::common::utility::TemplatedKeyValueHashMap<T>::clear();
				}
				
				//! remove by name
				void removeByName(const std::string api_name) {
					// call hash map method for remove an element by name
					chaos::common::utility::TemplatedKeyValueHashMap<T>::removeElement(api_name.c_str(), (uint32_t)api_name.size());
				}
			};
			
		}
	}
}
#endif
