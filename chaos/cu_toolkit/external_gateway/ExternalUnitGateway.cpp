/*
 *	ExternalUnitGateway.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/06/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/external_gateway/ExternalUnitGateway.h>
#include <chaos/cu_toolkit/external_gateway/http_adapter/HTTPAdapter.h>

using namespace chaos::cu::external_gateway;
using namespace chaos::common::utility;

#define INFO    INFO_LOG(ExternalUnitGateway)
#define DBG     DBG_LOG(ExternalUnitGateway)
#define ERR     ERR_LOG(ExternalUnitGateway)

ExternalUnitGateway::ExternalUnitGateway() {
    //add adapters
    map_protocol_adapter().insert(MapAdapterPair("http", ChaosSharedPtr<AbstractAdapter>(new http_adapter::HTTPAdapter())));
}

ExternalUnitGateway::~ExternalUnitGateway() {
    
}

void ExternalUnitGateway::init(void *init_data) throw (chaos::CException) {
   for(MapAdapterIterator it = map_protocol_adapter().begin(),
       end= map_protocol_adapter().end();
       it != end;
       it++) {
       InizializableService::initImplementation(*it->second, NULL, it->first, __PRETTY_FUNCTION__);
   }
}

void ExternalUnitGateway::deinit() throw (chaos::CException) {
    for(MapAdapterIterator it = map_protocol_adapter().begin(),
        end= map_protocol_adapter().end();
        it != end;
        it++) {
        CHAOS_NOT_THROW(InizializableService::deinitImplementation(*it->second, it->first, __PRETTY_FUNCTION__););
    }
}


int ExternalUnitGateway::registerEndpoint(const ExternalUnitEndpoint& endpoint) {
    if(getServiceState() != 1) return -1;
    return 0;
}

int ExternalUnitGateway::deregisterEndpoint(const ExternalUnitEndpoint& endpoint) {
    return 0;
}
