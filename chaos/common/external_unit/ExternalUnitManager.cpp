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
#include <chaos/common/external_unit/http_adapter/HTTPClientAdapter.h>
#include <chaos/common/external_unit/serialization/ExternalBSONExtJsonSerialization.h>

using namespace chaos::common::external_unit;
using namespace chaos::common::utility;

#define INFO    INFO_LOG(ExternalUnitManager)
#define DBG     DBG_LOG(ExternalUnitManager)
#define ERR     ERR_LOG(ExternalUnitManager)

ExternalUnitManager::ExternalUnitManager() {
    //add adapters
    map_protocol_adapter().insert(MapAdapterPair("http",PairAdapter(ChaosSharedPtr<AbstractServerAdapter>(new http_adapter::HTTPServerAdapter()),
                                                                    ChaosSharedPtr<AbstractClientAdapter>(new http_adapter::HTTPClientAdapter()))));
    
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
        InizializableService::initImplementation(*it->second.first, NULL, it->first, __PRETTY_FUNCTION__);
        InizializableService::initImplementation(*it->second.second, NULL, it->first, __PRETTY_FUNCTION__);
        
        it->second.first->registerEndpoint(echo_endpoint);
    }
}

void ExternalUnitManager::deinit() throw (chaos::CException) {
    LMapAdapterWriteLock wl = map_protocol_adapter.getWriteLockObject();
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end;
        it++) {
        it->second.first->deregisterEndpoint(echo_endpoint);
        CHAOS_NOT_THROW(InizializableService::deinitImplementation(*it->second.first, it->first, __PRETTY_FUNCTION__););
        CHAOS_NOT_THROW(InizializableService::deinitImplementation(*it->second.second, it->first, __PRETTY_FUNCTION__););
    }
}


int ExternalUnitManager::registerEndpoint(ExternalUnitServerEndpoint& endpoint) {
    if(getServiceState() != 1) return -1;
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    int err = 0;
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end &&
        err == 0;
        it++) {
        err = it->second.first->registerEndpoint(endpoint);
    }
    return err;
}

int ExternalUnitManager::deregisterEndpoint(ExternalUnitServerEndpoint& endpoint) {
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    int err = 0;
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end &&
        err == 0;
        it++) {
        err = it->second.first->deregisterEndpoint(endpoint);
    }
    return err;
}

int ExternalUnitManager::initilizeConnection(ExternalUnitClientEndpoint& endpoint,
                                             const std::string& protocol,
                                             const std::string& serialization,
                                             const std::string& destination) {
    if(getServiceState() != 1) return -1;
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    MapAdapterIterator it = map_protocol_adapter().find(protocol);
    if(it == map_protocol_adapter().end()) return -2;
    return it->second.second->addNewConnectionForEndpoint(&endpoint,
                                                          destination,
                                                          serialization);
}

int ExternalUnitManager::releaseConnection(ExternalUnitClientEndpoint& endpoint,
                                           const std::string& protocol) {
    if(getServiceState() != 1) return -1;
    LMapAdapterReadLock rl = map_protocol_adapter.getReadLockObject();
    MapAdapterIterator it = map_protocol_adapter().find(protocol);
    if(it == map_protocol_adapter().end()) return -2;
    return it->second.second->removeConnectionsFromEndpoint(&endpoint);
}

ChaosUniquePtr<serialization::AbstractExternalSerialization> ExternalUnitManager::getNewSerializationInstanceForType(const std::string& type) {
    LMapSerializerReadLock rl = map_serializer.getReadLockObject();
    MapSerializerIterator found_ser_it = map_serializer().find(type);
    if(found_ser_it == map_serializer().end()) return ChaosUniquePtr<serialization::AbstractExternalSerialization>();
    return ChaosUniquePtr<serialization::AbstractExternalSerialization>(found_ser_it->second->getInstance());
}
