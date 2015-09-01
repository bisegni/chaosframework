/*
 *	DirectIOPerformanceServerChannel.cpp
 *	!CHAOS
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


#include <chaos/common/utility/endianess.h>
#include <chaos/common/direct_io/channel/DirectIOPerformanceServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIOPerformanceServerChannel, DirectIOVirtualServerChannel);

DirectIOPerformanceServerChannel::DirectIOPerformanceServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIOPerformance_Channel_Index), handler(NULL) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

DirectIOPerformanceServerChannel::~DirectIOPerformanceServerChannel() {
	
}

void DirectIOPerformanceServerChannel::setHandler(DirectIOPerformanceServerChannel::DirectIOPerformanceServerChannelHandler *_handler) {
	handler = _handler;
}

int DirectIOPerformanceServerChannel::consumeDataPack(DirectIODataPack *dataPack,
                                                      DirectIODataPack *synchronous_answer,
                                                      DirectIODeallocationHandler **answer_header_deallocation_handler,
                                                      DirectIODeallocationHandler **answer_data_deallocation_handler) {
	CHAOS_ASSERT(handler)
	
    // the opcode
	opcode::PerformanceChannelOpcode  channel_opcode = static_cast<opcode::PerformanceChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
	
	switch (channel_opcode) {
		case opcode::PerformanceChannelOpcodeReqRoundTrip: {
            opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header = reinterpret_cast< opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr >(dataPack->channel_header_data);
			//reallign the pointer to the start of the key
			header->field.start_rt_ts = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.start_rt_ts);
			handler->handleReqRoundTripRequest(header);
            break;
        }
			
		case opcode::PerformanceChannelOpcodeRespRoundTrip:
			opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header = reinterpret_cast< opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr >(dataPack->channel_header_data);
			//reallign the pointer to the start of the key
			header->field.start_rt_ts = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.start_rt_ts);
			handler->handleRespRoundTripRequest(header);
			break;
	}
	
	//only data pack is deleted, header data and channel data are managed by hendler
	free(dataPack);
	
	return 0;
}
