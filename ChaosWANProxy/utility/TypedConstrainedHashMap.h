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
			protected chaos::common::utility::TemplatedKeyValueHashMap<T*> {
				
			public:
				//! construct the group with a name
				TypedConstrainedHashMap(uint32_t hash_power):common::utility::TemplatedKeyValueHashMap<T*>(hash_power){};
				
				//! default destructor
				virtual ~TypedConstrainedHashMap(){};
				
				//! add a class instance as api the name is got from the service name
				template <typename A, typename P>
				void addTypeWithParam(P param) {
					ChaosUniquePtr<INSTANCER_P1(A, T, P)> i(ALLOCATE_INSTANCER_P1(A, T, P));
					T *instance = i->getInstance(param);
					if(instance) {
						chaos::common::utility::TemplatedKeyValueHashMap<T*>::addElement(instance->getName().c_str(),
																						 (uint32_t)instance->getName().size(),
																						 instance);
					}
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
