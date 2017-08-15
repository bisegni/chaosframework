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

#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>

using namespace chaos::cu::driver_manager::driver;

OpcodeExternalCommandMapper::OpcodeExternalCommandMapper(AbstractRemoteIODriver *_remote_driver):
remote_driver(_remote_driver){CHAOS_ASSERT(remote_driver)}

OpcodeExternalCommandMapper::~OpcodeExternalCommandMapper() {}


int OpcodeExternalCommandMapper::sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                                chaos::common::data::CDWShrdPtr& message_response,
                                                uint32_t timeout) {
    CHAOS_ASSERT(remote_driver)
    return remote_driver->sendRawRequest(ChaosMoveOperator(message_data),
                                         message_response,
                                         timeout);
}


int OpcodeExternalCommandMapper::sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
    CHAOS_ASSERT(remote_driver)
    return remote_driver->sendRawMessage(ChaosMoveOperator(message_data));
}

const int OpcodeExternalCommandMapper::getNumberOfMaxConnection() const {
    CHAOS_ASSERT(remote_driver)
    return remote_driver->getNumberOfAcceptedConnection();
}
void OpcodeExternalCommandMapper::setNumberOfMaxConnection(int max_conenction) {
    CHAOS_ASSERT(remote_driver)
    remote_driver->setNumberOfAcceptedConnection(max_conenction);
}
