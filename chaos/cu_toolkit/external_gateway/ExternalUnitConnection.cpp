/*
 *	ExternalConnection.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/external_gateway/ExternalUnitConnection.h>
#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/global.h>

using namespace chaos::common::utility;

using namespace chaos::cu::external_gateway;

ExternalUnitConnection::ExternalUnitConnection(ExternalUnitEndpoint *_endpoint):
connection_identifier(UUIDUtil::generateUUIDLite()),
endpoint(_endpoint){
    endpoint->addConnection(*this);
}

ExternalUnitConnection::~ExternalUnitConnection() {
    endpoint->removeConnection(*this);
}

int ExternalUnitConnection::sendDataToEndpoint(const std::string& data) {
    CHAOS_ASSERT(endpoint);
    return endpoint->handleReceivedeMessage(connection_identifier, data);
}
