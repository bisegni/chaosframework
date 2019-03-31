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
#include "../DriverPoolManager.h"
#include "../batch/MDSBatchExecutor.h"

using namespace chaos::common::network;

using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::persistence;
using namespace chaos::service_common::persistence::data_access;

//! default constructor with the alias of the api
AbstractApi::AbstractApi(const std::string& name):
NamedService(name),
parent_group(NULL){}

//default destructor
AbstractApi::~AbstractApi(){deinit();}

void AbstractApi::init(void *init_data)  {}

void AbstractApi::deinit() {}

AbstractPersistenceDriver *AbstractApi::getPersistenceDriver() {
    return &DriverPoolManager::getInstance()->getPersistenceDrv();
}

MDSBatchExecutor *AbstractApi::getBatchExecutor() {
    return MDSBatchExecutor::getInstance();
}

NetworkBroker *AbstractApi::getNetworkBroker() {
    return NetworkBroker::getInstance();
}
