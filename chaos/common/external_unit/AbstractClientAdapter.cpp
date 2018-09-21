/*
 * Copyright 2012, 11/10/2017 INFN
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

#include <chaos/common/external_unit/AbstractClientAdapter.h>

using namespace chaos::common::external_unit;

AbstractClientAdapter::AbstractClientAdapter() {}

AbstractClientAdapter::~AbstractClientAdapter() {}

void AbstractClientAdapter::init(void *init_data)  {}

void AbstractClientAdapter::deinit()  {}

int AbstractClientAdapter::sendDataToEndpoint(ExternalUnitConnection& connection,
                                        chaos::common::data::CDBufferUniquePtr received_data) {
    return connection.sendDataToEndpoint(MOVE(received_data));
}
