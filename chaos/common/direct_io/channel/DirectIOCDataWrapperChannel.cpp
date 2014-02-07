//
//  File.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//


#include <chaos/common/direct_io/channel/DirectIOCDataWrapperChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;

#define DIOCDC_Channel_Index 1

#define CALL_SUPER_CONSTRUCTOR(n,i,p) \
channel::DirectIOVirtualChannel(std::string(n), i), \
channel::DirectIOVirtualClientChannel(std::string(n), i, p), \
channel::DirectIOVirtualServerChannel(std::string(n), i)

channel::DirectIOCDataWrapperChannel::DirectIOCDataWrapperChannel(DirectIOCDataWrapperChannelHandler *delegate):CALL_SUPER_CONSTRUCTOR("DirectIOCDataWrapperChannel", DIOCDC_Channel_Index, true){
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate((chaos::common::direct_io::DirectIOEndpointHandler*)this);
	
	//associate the exepternal delegate
	cdatawrapper_handler = delegate;
}

uint32_t channel::DirectIOCDataWrapperChannel::pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t tag, chaos_data::SerializationBuffer *data_pack) {
	DirectIODataPack *dio_data_pack = new DirectIODataPack();
	dio_data_pack->header.fields.route_addr = endpoint_idx;
	dio_data_pack->header.fields.channel_idx = DIOCDC_Channel_Index;
	dio_data_pack->header.fields.channel_tag = tag;
	dio_data_pack->data = (void*)data_pack->getBufferPtr();
	dio_data_pack->data_size = (uint32_t)data_pack->getBufferLen();
	return sendData(dio_data_pack);
}

void channel::DirectIOCDataWrapperChannel::consumeDataPack(DirectIODataPack *dataPack) {
	//call handler
	cdatawrapper_handler->consumeCDataWrapper(dataPack->header.fields.channel_tag, new chaos_data::CDataWrapper(static_cast<const char *>(dataPack->data)));
}