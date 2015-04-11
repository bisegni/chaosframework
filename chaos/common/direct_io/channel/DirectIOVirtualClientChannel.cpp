/*
 *	DirectIOVirtualChannel.cpp
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

#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

using namespace chaos::common::direct_io::channel;


DirectIOVirtualClientChannel::DirectIOVirtualClientChannel(std::string channel_name,
														   uint8_t channel_route_index):
DirectIOVirtualChannel(channel_name, channel_route_index),
header_deallocator(this) {}

DirectIOVirtualClientChannel::~DirectIOVirtualClientChannel() {}

int64_t DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPack *data_pack,
													   DirectIOSynchronousAnswer **synchronous_answer) {
	//set the endpoint that need the receive the pack on the other side
	//data_pack->header.dispatcher_header.fields.route_addr = endpoint;
	//set the channel route index within the endpoint
	data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
	
	//send pack
	//return DirectIOForwarderHandlerCaller(client_instance,forward_handler)(this, completeDataPack(data_pack, synchronous_answer != NULL ), synchronous_answer);
	return sendPriorityData(data_pack, header_deallocator, synchronous_answer);
}

int64_t DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPack *data_pack,
													   DirectIOClientDeallocationHandler *data_deallocator,
													   DirectIOSynchronousAnswer **synchronous_answer) {
	//set the endpoint that need the receive the pack on the other side
	//data_pack->header.dispatcher_header.fields.route_addr = endpoint;
	//set the channel route index within the endpoint
	data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
	
	//send pack
	//return DirectIOForwarderHandlerCaller(client_instance,forward_handler)(this, completeDataPack(data_pack, synchronous_answer != NULL ), synchronous_answer);
	return client_instance->sendPriorityData(completeChannnelDataPack(data_pack, synchronous_answer!=NULL), header_deallocator, data_deallocator, synchronous_answer);
}

int64_t DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPack *data_pack,
													  DirectIOSynchronousAnswer **synchronous_answer) {
	//set the endpoint that need the receive the pack on the other side
	//data_pack->header.dispatcher_header.fields.route_addr = endpoint;
	//set the channel route index within the endpoint
	data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
	
	//send pack
	//return DirectIOForwarderHandlerCaller(client_instance,forward_handler)(this, completeDataPack(data_pack, synchronous_answer != NULL ), synchronous_answer);
	return sendServiceData(data_pack, header_deallocator, synchronous_answer);
}

int64_t DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPack *data_pack,
													   DirectIOClientDeallocationHandler *data_deallocator,
													   DirectIOSynchronousAnswer **synchronous_answer) {
	//set the endpoint that need the receive the pack on the other side
	//data_pack->header.dispatcher_header.fields.route_addr = endpoint;
	//set the channel route index within the endpoint
	data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
	
	//send pack
	//return DirectIOForwarderHandlerCaller(client_instance,forward_handler)(this, completeDataPack(data_pack, synchronous_answer != NULL ), synchronous_answer);
	return client_instance->sendPriorityData(completeChannnelDataPack(data_pack, synchronous_answer!=NULL), header_deallocator, data_deallocator, synchronous_answer);
}

//! default header deallocator implementation
void DirectIOVirtualClientChannel::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
	free(sent_data_ptr);
}
