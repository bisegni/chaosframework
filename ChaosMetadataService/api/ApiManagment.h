/*
 *	ApiManagment.h
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
#ifndef __CHAOSFramework__ApiManagment__
#define __CHAOSFramework__ApiManagment__

#include "AbstractApiGroup.h"
#include "../persistence/AbstractPersistenceDriver.h"
#include "../batch/mds_service_batch.h"
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/network/NetworkBroker.h>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace api {
			//!typedef for the group list
            typedef boost::shared_ptr<AbstractApiGroup>          ApiGroupListElement;
			typedef std::vector< ApiGroupListElement >           ApiGroupList;
			typedef std::vector< ApiGroupListElement >::iterator ApiGroupListIterator;
			
			class ApiManagment:
			public chaos::common::utility::InizializableService {
				//! network broker pointer
				chaos::common::network::NetworkBroker *network_broker_instance;
				
                //! persistence driver instance
                persistence::AbstractPersistenceDriver *persistence_driver;
                
                //! batch executor engine
                batch::MDSBatchExecutor *batch_executor;
                
				//! api group list
				ApiGroupList installed_api_group_list;
				
				void clearGroupList();
			public:
				//! default consturctor
				ApiManagment(chaos::common::network::NetworkBroker *_network_broker_instance,
                             persistence::AbstractPersistenceDriver *_persistence_driver);
				
				//! default destructor
				~ApiManagment();
				
				//! install an instance of an api group
				void addApiAgroup(AbstractApiGroup *instance) {
					CHAOS_ASSERT(network_broker_instance)
					if(!instance) return;
					network_broker_instance->registerAction(instance);
					//we have an instance so we can register that action
					installed_api_group_list.push_back(boost::shared_ptr<AbstractApiGroup>(instance));
				}
				
				//! install a class as api group
				template<typename T>
				void addApiAgroup() {
					CHAOS_ASSERT(network_broker_instance)
					//allcoate the instsancer for the AbstractApi depending by the template
					std::auto_ptr<INSTANCER(T, AbstractApiGroup)> i(ALLOCATE_INSTANCER(T, AbstractApiGroup));
					
					//get api instance
					T *instance = (T*)i->getInstance();
					if(instance) {
						network_broker_instance->registerAction(instance);
						//we have an instance so we can register that action
						installed_api_group_list.push_back(boost::shared_ptr<AbstractApiGroup>(instance));
					}
				}
				
				void init(void* init_data) throw (chaos::CException);
				void deinit() throw (chaos::CException);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__ApiManagment__) */
