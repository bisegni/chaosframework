//
//  DirectIOCDataWrapperServerChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//


#include <chaos/common/direct_io/channel/DirectIOCDataWrapperServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;


DirectIOCDataWrapperServerChannel::DirectIOCDataWrapperServerChannel(std::string alias):DirectIOVirtualServerChannel(alias, DIOCDC_Channel_Index), cdatawrapper_handler(NULL) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIOCDataWrapperServerChannel::consumeDataPack(DirectIODataPack *dataPack) {
	CHAOS_ASSERT(cdatawrapper_handler)
	cdatawrapper_handler->consumeCDataWrapper(dataPack->header.fields.channel_tag, new chaos_data::CDataWrapper(static_cast<const char *>(dataPack->data)));
}


void DirectIOCDataWrapperServerChannel::setHandler(DirectIOCDataWrapperServerChannelHandler *handler) {
	//associate the exepternal delegate
	cdatawrapper_handler = handler;
}