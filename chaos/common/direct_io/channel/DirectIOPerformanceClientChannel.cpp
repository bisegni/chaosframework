/*
 *	DirectIOPerformanceClientChannel.cpp
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


#include <chaos/common/direct_io/channel/DirectIOPerformanceClientChannel.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace chaos::common::direct_io::channel;

DirectIOPerformanceClientChannel::DirectIOPerformanceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIOPerformance_Channel_Index, true)  {
	
}

DirectIOPerformanceClientChannel::~DirectIOPerformanceClientChannel() {

}

int64_t DirectIOPerformanceClientChannel::sendRoundTripMessage() {
	DirectIODataPack *data_pack = new DirectIODataPack();
	std::memset(data_pack, 0, sizeof(DirectIODataPack));
	
	//set opcode
	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::PerformanceChannelOpcodeRoundTrip);
	opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header = new opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip();
	
	boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration duration( time.time_of_day() );
	
	header->field.start_rt_ts = duration.total_microseconds();
	//rt_opcode_header->fields.
	//set the header
	//DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)PUT_HEADER_LEN)
	
	//send pack
	return client_instance->sendPriorityData(this, completeDataPack(data_pack));
}

void DirectIOPerformanceClientChannel::freeSentData(void *data,  DisposeSentMemoryInfo& dispose_memory_info) {
	//free all received data
	free(data);
}