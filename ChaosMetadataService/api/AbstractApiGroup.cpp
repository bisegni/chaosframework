/*
 *	AbstractApiGroup.cpp
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

#include "AbstractApiGroup.h"
using namespace chaos::common::utility;
using namespace chaos::metadata_service::api;

AbstractApiGroup::AbstractApiGroup(const std::string& name):
NamedService(name),
persistence_driver(NULL){
}

AbstractApiGroup::~AbstractApiGroup(){
}

void AbstractApiGroup::init(void *init_data) throw (chaos::CException) {
    persistence_driver = static_cast<persistence::AbstractPersistenceDriver*>(init_data);
    if(!persistence_driver) throw chaos::CException(-1, "No Persistence Driver has been set", __PRETTY_FUNCTION__);
    
    for(ApiListIterator it = api_instance.begin();
        it != api_instance.end();
        it++) {
        boost::shared_ptr<AbstractApi> api = *it;
        InizializableService::initImplementation(api.get(),
                                                 static_cast<void*>(persistence_driver),
                                                 api->getName(),
                                                 __PRETTY_FUNCTION__);
    }
}

void AbstractApiGroup::deinit()  throw (chaos::CException) {
    for(ApiListIterator it = api_instance.begin();
        it != api_instance.end();
        it++) {
        boost::shared_ptr<AbstractApi> api = *it;
        InizializableService::deinitImplementation(api.get(),
                                                   api->getName(),
                                                   __PRETTY_FUNCTION__);
    }

}