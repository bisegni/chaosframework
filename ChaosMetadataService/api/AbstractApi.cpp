/*
 *	AbstractApi.cpp
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

#include "AbstractApi.h"
using namespace chaos::common::network;

using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::persistence;
//! defaukt constructor with the alias of the api
AbstractApi::AbstractApi(const std::string& name):
NamedService(name),
subservice(NULL){}

//default destructor
AbstractApi::~AbstractApi(){}

void AbstractApi::init(void *init_data) throw (chaos::CException) {
    subservice = static_cast<ApiSubserviceAccessor*>(init_data);
    if(!subservice) throw chaos::CException(-1, "No Persistence Driver has been set", __PRETTY_FUNCTION__);
}

void AbstractApi::deinit()  throw (chaos::CException) {
    
}

chaos::service_common::persistence::data_access::AbstractPersistenceDriver *AbstractApi::getPersistenceDriver() {
    CHAOS_ASSERT(subservice)
    return subservice->persistence_driver.get();
}

MDSBatchExecutor *AbstractApi::getBatchExecutor() {
    CHAOS_ASSERT(subservice)
    return subservice->batch_executor.get();
}

NetworkBroker *AbstractApi::getNetworkBroker() {
    CHAOS_ASSERT(subservice)
    return subservice->network_broker_service.get();
}