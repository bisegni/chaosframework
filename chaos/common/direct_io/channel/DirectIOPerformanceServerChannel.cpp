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
    int err = -1;
    // the opcode
	opcode::PerformanceChannelOpcode  channel_opcode = static_cast<opcode::PerformanceChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
	
	switch (channel_opcode) {
		case opcode::PerformanceChannelOpcodeReqRoundTrip: {
            opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header = reinterpret_cast< opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr >(dataPack->channel_header_data);
			//reallign the pointer to the start of the key
			header->field.start_rt_ts = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.start_rt_ts);
			handler->handleReqRoundTripRequest(header);
            err = 0;
            break;
        }
			
		case opcode::PerformanceChannelOpcodeRespRoundTrip:
			opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header = reinterpret_cast< opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr >(dataPack->channel_header_data);
			//reallign the pointer to the start of the key
			header->field.start_rt_ts = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.start_rt_ts);
			handler->handleRespRoundTripRequest(header);
            err = 0;
			break;
	}
	
	//only data pack is deleted, header data and channel data are managed by handler
	free(dataPack);
	
	return err;
}
