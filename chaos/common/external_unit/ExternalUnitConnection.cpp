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

#include <chaos/common/external_unit/ExternalUnitConnection.h>

#include <chaos/common/external_unit/AbstractAdapter.h>
#include <chaos/common/external_unit/UnitEndpoint.h>

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/global.h>

using namespace chaos::common::utility;

using namespace chaos::common::external_unit;

ExternalUnitConnection::ExternalUnitConnection(AbstractAdapter *_adapter,
                                               UnitEndpoint *_endpoint,
                                               ChaosUniquePtr<chaos::common::external_unit::serialization::AbstractExternalSerialization> _serializer_adapter):
connection_identifier(UUIDUtil::generateUUIDLite()),
online(false),
accepted_state(-1),
adapter(_adapter),
endpoint(_endpoint),
serializer_adapter(ChaosMoveOperator(_serializer_adapter)) {
    CHAOS_ASSERT(adapter && endpoint && serializer_adapter.get());
    endpoint->addConnection(*this);
}

ExternalUnitConnection::~ExternalUnitConnection() {
    endpoint->removeConnection(*this);
}

int ExternalUnitConnection::sendDataToEndpoint(chaos::common::data::CDBufferUniquePtr reecived_data) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> dmessage = serializer_adapter->deserialize(*reecived_data);
    if(!dmessage.get()) return -1;
    return endpoint->handleReceivedeMessage(connection_identifier, ChaosMoveOperator(dmessage));
}

int ExternalUnitConnection::sendData(chaos::common::data::CDWUniquePtr data,
                                     const EUCMessageOpcode opcode) {
    CHAOS_ASSERT(data.get());
    return adapter->sendDataToConnection(connection_identifier,
                                         serializer_adapter->serialize(*data),
                                         opcode);
}

void ExternalUnitConnection::closeConnection() {
    adapter->closeConnection(connection_identifier);
}

const std::string ExternalUnitConnection::getEndpointIdentifier() const {
    return endpoint->getIdentifier();
}
