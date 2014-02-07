//
//  DirectIOVirtualChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 07/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

using namespace chaos::common::direct_io::channel;

DirectIOVirtualChannel::DirectIOVirtualChannel(std::string _channel_name, uint8_t __channel_route_index):NamedService(_channel_name), channel_route_index(__channel_route_index) {
	
}

DirectIOVirtualChannel::~DirectIOVirtualChannel() {
	
}

uint8_t DirectIOVirtualChannel::getChannelRouterIndex() {
	return channel_route_index;
}
