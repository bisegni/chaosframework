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
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;

#define DirectIOServerEndpoint_LOG_HEAD "[DirectIOServerEndpoint-"<<endpoint_route_index<<"] - "

#define DIOSE_LAPP_ LAPP_ << DirectIOServerEndpoint_LOG_HEAD
#define DIOSE_LDBG_ LDBG_ << DirectIOServerEndpoint_LOG_HEAD
#define DIOSE_LERR_ LERR_ << DirectIOServerEndpoint_LOG_HEAD

#define INCREASE_COUNTER_OFF_USAGE	while(!pack_receiving){};pack_receiving++;
#define DECREASE_COUNTER_OFF_USAGE	while(!pack_receiving){};pack_receiving--;
#define LOCK_COUNTER_OFF_USAGE		pack_receiving = 0
#define UNLOCK_COUNTER_OFF_USAGE	pack_receiving = 1

DirectIOServerEndpoint::DirectIOServerEndpoint():
endpoint_route_index(0),
server_public_interface(NULL) {
    //allocate slot memory
    channel_slot = (channel::DirectIOVirtualServerChannel**)calloc(1, (sizeof(channel::DirectIOVirtualServerChannel*)*MAX_ENDPOINT_CHANNEL));
    //clean all memory
    //std::memset(channel_slot, 0, sizeof(channel::DirectIOVirtualServerChannel**)*MAX_ENDPOINT_CHANNEL);
}

DirectIOServerEndpoint::~DirectIOServerEndpoint() {
    if(channel_slot) free(channel_slot);
}

uint16_t DirectIOServerEndpoint::getRouteIndex() {
    return endpoint_route_index;
}

DirectIOServerPublicInterface * DirectIOServerEndpoint::getPublicServerInterface() const {
    return server_public_interface;
}

std::string DirectIOServerEndpoint::getUrl() {
    if(!server_public_interface) return std::string("");
    //return the url concatenating he server url with the endpoint divided by pipe chacracter
    return boost::str( boost::format("%1%|%2%") % server_public_interface->getUrl() % endpoint_route_index);
}

//! Add a new channel instantiator
channel::DirectIOVirtualServerChannel *DirectIOServerEndpoint::registerChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance) {
    if(!channel_instance) return NULL;
    // gest exsclusive access
    DIOSE_LDBG_ << "Register channel " << channel_instance->getName() << " with route index " << (int)channel_instance->getChannelRouterIndex();
    if(channel_instance->getChannelRouterIndex() > (MAX_ENDPOINT_CHANNEL-1)) return NULL;
    ChaosWriteLock rl(shared_mutex);
    channel_slot[channel_instance->getChannelRouterIndex()] = channel_instance;
    return channel_instance;
}

//! Dispose the channel
void DirectIOServerEndpoint::deregisterChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance) {
    if(!channel_instance) return;
    if(channel_instance->getChannelRouterIndex() > (MAX_ENDPOINT_CHANNEL-1)) return;
    ChaosWriteLock rl(shared_mutex);
    channel_slot[channel_instance->getChannelRouterIndex()] = NULL;
}


// New channel allocation by name
channel::DirectIOVirtualServerChannel *DirectIOServerEndpoint::getNewChannelInstance(std::string channel_name) throw (CException) {
    channel::DirectIOVirtualServerChannel *channel = ObjectFactoryRegister<channel::DirectIOVirtualServerChannel>::getInstance()->getNewInstanceByName(channel_name);
    registerChannelInstance(channel);
    return channel;
}

// New channel allocation by name
void DirectIOServerEndpoint::releaseChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance) throw (CException) {
    deregisterChannelInstance(channel_instance);
    if(channel_instance) delete(channel_instance);
}

// Event for a new data received
int DirectIOServerEndpoint::priorityDataReceived(DirectIODataPackSPtr data_pack,
                                                 DirectIODataPackSPtr& synchronous_answer) {
    int err = 0;
    ChaosReadLock rl(shared_mutex);
    if(channel_slot[data_pack->header.dispatcher_header.fields.channel_idx]) {
        err =  channel_slot[data_pack->header.dispatcher_header.fields.channel_idx]->server_channel_delegate->consumeDataPack(ChaosMoveOperator(data_pack),
                                                                                                                              synchronous_answer);
    }
    return err;
}

// Event for a new data received
int DirectIOServerEndpoint::serviceDataReceived(DirectIODataPackSPtr data_pack,
                                                DirectIODataPackSPtr& synchronous_answer) {
    int err = 0;
    ChaosReadLock rl(shared_mutex);
    if(channel_slot[data_pack->header.dispatcher_header.fields.channel_idx]) {
        err = channel_slot[data_pack->header.dispatcher_header.fields.channel_idx]->server_channel_delegate->consumeDataPack(ChaosMoveOperator(data_pack),
                                                                                                                             synchronous_answer);
    }
    return err;
}
