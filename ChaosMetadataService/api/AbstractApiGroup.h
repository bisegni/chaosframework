/*
 *	AbstractGroup.h
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
#ifndef CHAOSFramework_AbstractGroup_h
#define CHAOSFramework_AbstractGroup_h

#include "AbstractApi.h"
#include "../mds_types.h"

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/chaos_types.h>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace api {
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<AbstractApi>, ApiList)

			class AbstractApiGroup:
			public common::utility::NamedService,
            public chaos::common::utility::InizializableService,
			public chaos::DeclareAction {
                friend class AbstractApi;
                //! the instace of the persistence driver
                ApiSubserviceAccessor *subservice;
                
                //! list of all installed api in the group
                ApiList api_instance;
            protected:
                //! install a class as api
                /*!
                 The alias of the action to be call si got by api itself
                 */
                template<typename T>
                boost::shared_ptr<T> getNewApiInstance() {
                    //allcoate the instsancer for the AbstractApi depending by the template
                    std::auto_ptr<INSTANCER(T, AbstractApi)> i(ALLOCATE_INSTANCER(T, AbstractApi));
                    
                    //get api instance
                    boost::shared_ptr<T> instance((T*)i->getInstance());
                    if(instance.get()) {
                        //we have an instance so we can initilize it
                        InizializableService::initImplementation(instance.get(),
                                                                 static_cast<void*>(subservice),
                                                                 instance->getName(),
                                                                 __PRETTY_FUNCTION__);
                    }
                }
                
			public:
				AbstractApiGroup(const std::string& name);
                
				~AbstractApiGroup();
				
				//! install a class as api
				/*!
				 The alias of the action to be call si got by api itself
				 */
				template<typename T>
				void addApi() {
					//allcoate the instsancer for the AbstractApi depending by the template
					std::auto_ptr<INSTANCER(T, AbstractApi)> i(ALLOCATE_INSTANCER(T, AbstractApi));
					
					//get api instance
					T *instance = (T*)i->getInstance();
					if(instance) {
						//we have an instance so we can register that action
						api_instance.push_back(boost::shared_ptr<AbstractApi>(instance));
						DeclareAction::addActionDescritionInstance<T>(instance,
																	  &T::execute,
																	  getName().c_str(),
																	  instance->getName().c_str(),
																	  instance->getName().c_str(),
                                                                      true);
					}
				}
                
                
                void init(void *init_data) throw (chaos::CException);
                
                void deinit()  throw (chaos::CException);
			};
			
		}
	}
}
#endif
