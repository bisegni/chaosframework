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
#include <boost/bind.hpp>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

using namespace chaos::common::direct_io::channel;

void dummy_vsf(chaos::common::direct_io::DirectIODataPack *data_pack) { /* has differing behaviour */ }

DirectIOVirtualServerChannel::DirectIOVirtualServerChannel(std::string channel_name,
                                                           uint8_t channel_route_index):
DirectIOVirtualChannel(channel_name,
                       channel_route_index),
endpoint_route_index(0){
	clearDelegate();
}
DirectIOVirtualServerChannel::~DirectIOVirtualServerChannel() {
	
}

void DirectIOVirtualServerChannel::setDelegate(ServerChannelDelegate delegate_function) {
	server_channel_delegate = delegate_function;
}

void DirectIOVirtualServerChannel::clearDelegate() {
	server_channel_delegate = NULL;
}

uint16_t DirectIOVirtualServerChannel::getEndpointRouteIndex() {
	return endpoint_route_index;
}