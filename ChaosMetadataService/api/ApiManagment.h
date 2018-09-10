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
#ifndef __CHAOSFramework__ApiManagment__
#define __CHAOSFramework__ApiManagment__

#include "AbstractApiGroup.h"
#include "../mds_types.h"
#include "../batch/mds_service_batch.h"

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/network/NetworkBroker.h>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace api {
			//!typedef for the group list
            typedef ChaosSharedPtr<AbstractApiGroup>          ApiGroupListElement;
			typedef std::vector< ApiGroupListElement >           ApiGroupList;
			typedef std::vector< ApiGroupListElement >::iterator ApiGroupListIterator;
			
			class ApiManagment:
			public chaos::common::utility::InizializableService {
				
                //! pointe to the root subservice struct
                ApiSubserviceAccessor *subservices;
                
				//! api group list
				ApiGroupList installed_api_group_list;
				
				void clearGroupList();
			public:
				//! default consturctor
				ApiManagment();
				
				//! default destructor
				~ApiManagment();
				
				//! install an instance of an api group
				void addApiAgroup(AbstractApiGroup *instance) {
					CHAOS_ASSERT(subservices)
					if(!instance) return;
					subservices->network_broker_service->registerAction(instance);
					//we have an instance so we can register that action
					installed_api_group_list.push_back(ChaosSharedPtr<AbstractApiGroup>(instance));
				}
				
				//! install a class as api group
				template<typename T>
				void addApiAgroup() {
					CHAOS_ASSERT(subservices)
					//allcoate the instsancer for the AbstractApi depending by the template
					ChaosUniquePtr<INSTANCER(T, AbstractApiGroup)> i(ALLOCATE_INSTANCER(T, AbstractApiGroup));
					
					//get api instance
					T *instance = (T*)i->getInstance();
					if(instance) {
						subservices->network_broker_service->registerAction(instance);
						//we have an instance so we can register that action
						installed_api_group_list.push_back(ChaosSharedPtr<AbstractApiGroup>(instance));
					}
				}
				
				void init(void* init_data) ;
				void deinit() ;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__ApiManagment__) */
