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

#include "AbstractApiGroup.h"

using namespace chaos::common::utility;
using namespace chaos::metadata_service::api;

AbstractApiGroup::AbstractApiGroup(const std::string& name):
NamedService(name),
subservice(NULL){}

AbstractApiGroup::~AbstractApiGroup(){}

void AbstractApiGroup::init(void *init_data) throw (chaos::CException) {
    subservice = static_cast<ApiSubserviceAccessor*>(init_data);
    if(!subservice) throw chaos::CException(-1, "No subsystem has been set", __PRETTY_FUNCTION__);
    
    for(ApiListIterator it = api_instance.begin();
        it != api_instance.end();
        it++) {
        ChaosSharedPtr<AbstractApi> api = *it;
        
        //initilize api
        InizializableService::initImplementation(api.get(),
                                                 static_cast<void*>(subservice),
                                                 api->getName(),
                                                 __PRETTY_FUNCTION__);
        //connect parent group to api
        api->parent_group = this;
    }
}

void AbstractApiGroup::deinit()  throw (chaos::CException) {
}
