/*
 *	AbstractGroup.h
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
#ifndef CHAOSFramework_AbstractGroup_h
#define CHAOSFramework_AbstractGroup_h

#include "AbstractApi.h"
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/action/DeclareAction.h>

#include <vector>

namespace chaos {
	namespace metadata_service {
		namespace api {
			
			class AbstractGroup:
			public common::utility::NamedService,
			public chaos::DeclareAction {
				std::vector<AbstractApi*> api_instance;
			public:
				AbstractGroup(const std::string& name):NamedService(name){};
				~AbstractGroup(){};
				
				template<typename T>
				void addApi() {
					std::auto_ptr<INSTANCER(T, AbstractApi)> i(ALLOCATE_INSTANCER(T, AbstractApi));
					T *instance = (T*)i->getInstance();
					if(instance) {
						api_instance.push_back(instance);
						DeclareAction::addActionDescritionInstance<T>(instance,
																	  &T::execute,
																	  getName().c_str(),
																	  instance->getName().c_str(),
																	  instance->getName().c_str());
					}
				}
			};
			
		}
	}
}
#endif
