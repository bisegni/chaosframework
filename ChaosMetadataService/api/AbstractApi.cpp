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

#include "AbstractApi.h"

using namespace chaos::common::network;

using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::persistence;
//! defaukt constructor with the alias of the api
AbstractApi::AbstractApi(const std::string& name):
NamedService(name),
subservice(NULL),
parent_group(NULL){}

//default destructor
AbstractApi::~AbstractApi(){deinit();}

void AbstractApi::init(void *init_data) throw (chaos::CException) {
    subservice = static_cast<ApiSubserviceAccessor*>(init_data);
    if(!subservice) throw chaos::CException(-1, "No Persistence Driver has been set", __PRETTY_FUNCTION__);
}

void AbstractApi::deinit()  throw (chaos::CException) {}

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
    return subservice->network_broker_service;
}
