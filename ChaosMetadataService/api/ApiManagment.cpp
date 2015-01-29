/*
 *	ApiManagment.cpp
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
#include "ApiManagment.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/foreach.hpp>

using namespace chaos::common::utility;
using namespace chaos::metadata_service::api;

//! default consturctor
ApiManagment::ApiManagment(chaos::common::network::NetworkBroker *_network_broker_instance,
                           persistence::AbstractPersistenceDriver *_persistence_driver):
network_broker_instance(_network_broker_instance),
persistence_driver(_persistence_driver){}

//! default destructor
ApiManagment::~ApiManagment() {
	clearGroupList();
}

void ApiManagment::init(void* init_data) throw (chaos::CException) {
    if(!persistence_driver) throw chaos::CException(-1, "NO persistence driver set", __PRETTY_FUNCTION__);
    
	//register all available api group
	std::vector<std::string> all_registered_groups;
	ObjectFactoryRegister<AbstractApiGroup>::getInstance()->getAllRegisteredName(all_registered_groups);
	BOOST_FOREACH( std::string name, all_registered_groups ) {
        //get the api group instance
		AbstractApiGroup * instance = ObjectFactoryRegister<AbstractApiGroup>::getInstance()->getNewInstanceByName(name);
        
        //initialize the api group
        InizializableService::initImplementation(instance,
                                                 static_cast<void*>(persistence_driver),
                                                 instance->getName(),
                                                 __PRETTY_FUNCTION__);
        //register the api group
		addApiAgroup(instance);
	}
}
void ApiManagment::deinit() throw (chaos::CException) {
	clearGroupList();
}

void ApiManagment::clearGroupList() {
	CHAOS_ASSERT(network_broker_instance)
	for(ApiGroupListIterator it = installed_api_group_list.begin();
		it != installed_api_group_list.end();
		it++) {
        //get api group reference
        ApiGroupListElement group = *it;
        
        //deregister the action
		network_broker_instance->deregisterAction((*it).get());
        
        //initialize the api group
        InizializableService::deinitImplementation(group.get(),
                                                   group->getName(),
                                                   __PRETTY_FUNCTION__);
	}
}