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

#include <chaos/common/global.h>
#include <chaos/common/external_unit/ExternalUnitManager.h>
#include <chaos/common/external_unit/http_adapter/HTTPServerAdapter.h>

#include <chaos/common/external_unit/serialization/ExternalBSONExtJsonSerialization.h>

using namespace chaos::common::external_unit;
using namespace chaos::common::utility;

#define INFO    INFO_LOG(ExternalUnitManager)
#define DBG     DBG_LOG(ExternalUnitManager)
#define ERR     ERR_LOG(ExternalUnitManager)

ExternalUnitManager::ExternalUnitManager() {
    //add adapters
    map_protocol_adapter().insert(MapAdapterPair("http", ChaosSharedPtr<AbstractAdapter>(new http_adapter::HTTPServerAdapter())));
    
    //!add serializer
    installSerializerInstancer<serialization::ExternalBSONExtJsonSerialization>();
}

ExternalUnitManager::~ExternalUnitManager() {
    
}

void ExternalUnitManager::init(void *init_data) throw (chaos::CException) {
    LMapAdapterWriteLock wl = map_protocol_adapter.getWriteLockObject();
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end;
        it++) {
        InizializableService::initImplementation(*it->second, NULL, it->first, __PRETTY_FUNCTION__);
        it->second->registerEndpoint(echo_endpoint);
    }
}

void ExternalUnitManager::deinit() throw (chaos::CException) {
    LMapAdapterWriteLock wl = map_protocol_adapter.getWriteLockObject();
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end;
        it++) {
        CHAOS_NOT_THROW(InizializableService::deinitImplementation(*it->second, it->first, __PRETTY_FUNCTION__););
    }
}


int ExternalUnitManager::registerEndpoint(ExternalUnitEndpoint& endpoint) {
    if(getServiceState() != 1) return -1;
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    int err = 0;
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end &&
        err == 0;
        it++) {
        err = it->second->registerEndpoint(endpoint);
    }
    return err;
}

int ExternalUnitManager::deregisterEndpoint(ExternalUnitEndpoint& endpoint) {
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    int err = 0;
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end &&
        err == 0;
        it++) {
        err = it->second->deregisterEndpoint(endpoint);
    }
    return err;
}

int ExternalUnitManager::initilizeConnection(ExternalUnitEndpoint& endpoint,
                                             const std::string& protocol,
                                             const std::string& serialization,
                                             const std::string& destination) {
    return 0;
}

int ExternalUnitManager::releaseConnection(ExternalUnitEndpoint& endpoint) {
    return 0;
}

ChaosUniquePtr<serialization::AbstractExternalSerialization> ExternalUnitManager::getNewSerializationInstanceForType(const std::string& type) {
    LMapSerializerReadLock rl = map_serializer.getReadLockObject();
    MapSerializerIterator found_ser_it = map_serializer().find(type);
    if(found_ser_it == map_serializer().end()) return ChaosUniquePtr<serialization::AbstractExternalSerialization>();
    return ChaosUniquePtr<serialization::AbstractExternalSerialization>(found_ser_it->second->getInstance());
}
