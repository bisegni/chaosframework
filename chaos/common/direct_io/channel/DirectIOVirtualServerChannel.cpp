/*
 *	DirectIOVirtualServerChannel.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include <boost/bind.hpp>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

using namespace chaos::common::direct_io::channel;

void dummy_vsf(chaos::common::direct_io::DirectIODataPack *data_pack) { /* has differing behaviour */ }

DirectIOVirtualServerChannel::DirectIOVirtualServerChannel(std::string channel_name, uint8_t channel_route_index):DirectIOVirtualChannel(channel_name, channel_route_index) {
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