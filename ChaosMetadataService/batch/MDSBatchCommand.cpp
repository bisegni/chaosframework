/*
 *  BatchCommand.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#include "MDSBatchCommand.h"

using namespace chaos::common::network;
using namespace chaos::metadata_service::batch;
using namespace chaos::common::batch_command;
#define MDSBC_INFO INFO_LOG(MDSBatchCommand)
#define MDSBC_DBG  INFO_DBG(MDSBatchCommand)
#define MDSBC_ERR  INFO_ERR(MDSBatchCommand)

//! default constructor
MDSBatchCommand::MDSBatchCommand():
BatchCommand(){
    
}

//! default destructor
MDSBatchCommand::~MDSBatchCommand() {
    
}

chaos::common::message::MessageChannel*
MDSBatchCommand::getNewMessageChannel() {
    CHAOS_ASSERT(network_broker)
    return network_broker->getRawMessageChannelFromAddress();
}

//! return a device message channel for a specific node address
chaos::common::message::DeviceMessageChannel*
MDSBatchCommand::getNewDeviceMessageChannelForAddress(chaos::common::network::CDeviceNetworkAddress *device_network_address) {
    CHAOS_ASSERT(network_broker)
    return network_broker->getDeviceMessageChannelFromAddress(device_network_address);
}

void
MDSBatchCommand::releaseChannel(chaos::common::message::MessageChannel *message_channel) {
    CHAOS_ASSERT(network_broker)
    network_broker->disposeMessageChannel(message_channel);
}

// return the implemented handler
uint8_t MDSBatchCommand::implementedHandler() {
    return  HandlerType::HT_Set|
            HandlerType::HT_Correlation|
            HandlerType::HT_Acquisition;
}

// inherited method
void MDSBatchCommand::setHandler(chaos_data::CDataWrapper *data) {
    BC_EXEC_RUNNIG_PROPERTY
}

// inherited method
void MDSBatchCommand::acquireHandler() {
    
}

// inherited method
void MDSBatchCommand::ccHandler() {
    
}

// inherited method
bool MDSBatchCommand::timeoutHandler() {
    return true;
}