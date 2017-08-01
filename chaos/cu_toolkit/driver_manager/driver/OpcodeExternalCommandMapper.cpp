/*
 *	OpcodeExternalCommandMapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/2017 INFN, National Institute of Nuclear Physics
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
