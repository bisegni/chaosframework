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

#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

using namespace chaos::common::direct_io::channel;


DirectIOVirtualClientChannel::DirectIOVirtualClientChannel(std::string channel_name,
                                                           uint8_t channel_route_index):
DirectIOVirtualChannel(channel_name, channel_route_index),
forward_handler(NULL),
header_deallocator(this),
client_instance(NULL){}

DirectIOVirtualClientChannel::~DirectIOVirtualClientChannel() {}

int64_t DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPack *data_pack,
                                                       DirectIODataPack **synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //send pack
    //return DirectIOForwarderHandlerCaller(client_instance,forward_handler)(this, completeDataPack(data_pack, synchronous_answer != NULL ), synchronous_answer);
    return sendPriorityData(data_pack, header_deallocator, synchronous_answer);
}

int64_t DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPack *data_pack,
                                                       DirectIODeallocationHandler *data_deallocator,
                                                       DirectIODataPack **synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    //send pack
    int64_t err = client_instance->sendPriorityData(completeChannnelDataPack(data_pack, synchronous_answer!=NULL), header_deallocator, data_deallocator, synchronous_answer);
    if(!err){
        if(synchronous_answer &&
           *synchronous_answer) {
            //convert default DirectIO hader to little endian
            DIRECT_IO_DATAPACK_TO_ENDIAN((*synchronous_answer))
            //report api error as function error
            err = (*synchronous_answer)->header.dispatcher_header.fields.err;
        }
    } else {
        //bad eeror we want an aswer bu received nothing
        err = -10000;
    }
    return err;
}

int64_t DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPack *data_pack,
                                                      DirectIODataPack **synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //send pack
    return sendServiceData(data_pack, header_deallocator, synchronous_answer);
}

int64_t DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPack *data_pack,
                                                      DirectIODeallocationHandler *data_deallocator,
                                                      DirectIODataPack **synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    //send pack
    int64_t err = client_instance->sendPriorityData(completeChannnelDataPack(data_pack, synchronous_answer!=NULL), header_deallocator, data_deallocator, synchronous_answer);
    if(!err){
        if(synchronous_answer &&
           *synchronous_answer) {
            //convert default DirectIO hader to little endian
            DIRECT_IO_DATAPACK_TO_ENDIAN((*synchronous_answer))
            //report api error as function error
            err = (*synchronous_answer)->header.dispatcher_header.fields.err;
        }
    }
    return err;
}

//! default header deallocator implementation
void DirectIOVirtualClientChannel::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
    free(sent_data_ptr);
}
