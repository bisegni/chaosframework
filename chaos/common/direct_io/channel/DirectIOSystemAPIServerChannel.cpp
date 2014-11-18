/*
 *	DirectIOSystemAPIServerChannel.cpp
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

#include <chaos/common/utility/endianess.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>

#include <boost/algorithm/string.hpp>

namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;


DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIOSystemAPI_Channel_Index),
handler(NULL) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIOSystemAPIServerChannel::setHandler(DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler *_handler) {
	handler = _handler;
}

int DirectIOSystemAPIServerChannel::consumeDataPack(DirectIODataPack *dataPack,
													DirectIOSynchronousAnswerPtr synchronous_answer) {
	CHAOS_ASSERT(handler)
	int err = 0;
	// the opcode
	opcode::SystemAPIChannelOpcode  channel_opcode = static_cast<opcode::SystemAPIChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
	
	switch (channel_opcode) {
		case opcode::SystemAPIChannelOpcodeNewNewSnapshootDataset: {
			//passed unique produce key vector
			std::vector<std::string> snapped_producer_keys;
			
			//the result of the api
			DirectIOSystemAPINewSnapshootResult *api_result = (DirectIOSystemAPINewSnapshootResult*)std::calloc(sizeof(DirectIOSystemAPINewSnapshootResult), 1) ;
			
			//set the answer pointer
			synchronous_answer->answer_data = api_result;
			synchronous_answer->answer_size = sizeof(DirectIOSystemAPINewSnapshootResult);
			
			//get the header
			opcode_headers::DirectIOSystemAPIChannelOpcodeNewSnapshootHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNewSnapshootHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
			
			if(header->field.producer_key_set_len) {
				//recreate the array of producer key set
				std::string concatenated_keys((const char*)dataPack->channel_data, header->field.producer_key_set_len);
				//split the concatenated string
				boost::split( snapped_producer_keys, concatenated_keys, is_any_of(","), token_compress_on );
			}
			//call the handler
			handler->consumeNewSnapshotEvent(header, snapped_producer_keys, *api_result);
			//fix endianes into api result
			api_result->result_field.error = TO_LITTE_ENDNS_NUM(int32_t, api_result->result_field.error);
			break;
		}
			
		default:
			break;
	}
	
	//only data pack is deleted, header and data of the channel are managed by handler
	free(dataPack);
	
	//return no result
	return err;
}
