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


#include <chaos/common/direct_io/channel/DirectIOPerformanceClientChannel.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace chaos::common::direct_io::channel;

DEFINE_CLASS_FACTORY(DirectIOPerformanceClientChannel, DirectIOVirtualClientChannel);

//define the static deallocator class
DirectIOPerformanceClientChannel::DirectIOPerformanceClientChannelDeallocator DirectIOPerformanceClientChannel::STATIC_DirectIOPerformanceClientChannelDeallocator;


DirectIOPerformanceClientChannel::DirectIOPerformanceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIOPerformance_Channel_Index)  {
	//associate the default static allocator
	header_deallocator = &STATIC_DirectIOPerformanceClientChannelDeallocator;
}

DirectIOPerformanceClientChannel::~DirectIOPerformanceClientChannel() {

}

int64_t DirectIOPerformanceClientChannel::sendRoundTripMessage() {
//    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(1, sizeof(DirectIODataPack));
//    //std::memset(data_pack, 0, sizeof(DirectIODataPack));
//
//    //set opcode
//    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::PerformanceChannelOpcodeReqRoundTrip);
//    opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header =
//    (opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr)calloc(sizeof(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip), 1);
//
//
//    boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
//    boost::posix_time::time_duration duration( time.time_of_day() );
//
//    header->field.start_rt_ts = TO_LITTEL_ENDNS_NUM(uint64_t, duration.total_microseconds());
//    //rt_opcode_header->fields.
//    //set the header
//    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, header, (uint32_t)sizeof(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip))
//
//    //send pack
//    return sendServiceData(data_pack);
    return -1;
}

int64_t DirectIOPerformanceClientChannel::answerRoundTripMessage(uint64_t received_ts) {
//    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(1, sizeof(DirectIODataPack));
//    //std::memset(data_pack, 0, sizeof(DirectIODataPack));
//
//    //set opcode
//    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::PerformanceChannelOpcodeRespRoundTrip);
//    opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr header =
//    (opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr)calloc(sizeof(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip), 1);
//
//    boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
//    boost::posix_time::time_duration duration( time.time_of_day() );
//
//    header->field.start_rt_ts = received_ts;
//    header->field.receiver_rt_ts = TO_LITTEL_ENDNS_NUM(uint64_t, duration.total_microseconds());
//    //rt_opcode_header->fields.
//    //set the header
//    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, header, (uint32_t)sizeof(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip))
//
//    //send pack
//    return sendServiceData(data_pack);
    return -1;
}

int64_t DirectIOPerformanceClientChannel::answerRoundTripMessage(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr received_header) {
//    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(1, sizeof(DirectIODataPack));
//    //std::memset(data_pack, 0, sizeof(DirectIODataPack));
//    
//    //set opcode
//    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::PerformanceChannelOpcodeRespRoundTrip);
//    
//    boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
//    boost::posix_time::time_duration duration( time.time_of_day() );
//    
//    received_header->field.receiver_rt_ts =TO_LITTEL_ENDNS_NUM(uint64_t, duration.total_microseconds());
//    //rt_opcode_header->fields.
//    //set the header
//    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, received_header, (uint32_t)sizeof(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTrip))
//    
//    //send pack
//    return sendServiceData(data_pack);
    return -1;
}

//! default data deallocator implementation
void DirectIOPerformanceClientChannel::DirectIOPerformanceClientChannelDeallocator::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
	free(sent_data_ptr);
}
