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

int DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    DirectIOForwarderPtrLOWriteLock wl = client_instance.getWriteLockObject();
    completeChannnelDataPack(data_pack.get(), false);
    //send pack
    return client_instance()->sendPriorityData(ChaosMoveOperator(data_pack));
}

int DirectIOVirtualClientChannel::sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                   chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    DirectIOForwarderPtrLOWriteLock wl = client_instance.getWriteLockObject();
    completeChannnelDataPack(data_pack.get(), true);
    //send pack
    int err = client_instance()->sendPriorityData(ChaosMoveOperator(data_pack),
                                                  synchronous_answer);
    if(!err){
        if(synchronous_answer &&
           synchronous_answer->channel_data) {
            //convert default DirectIO hader to little endian
            DIRECT_IO_DATAPACK_TO_ENDIAN(synchronous_answer)
            //report api error as function error
            err = synchronous_answer->header.dispatcher_header.fields.err;
        }
    }
    return err;
}

int DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    DirectIOForwarderPtrLOWriteLock wl = client_instance.getWriteLockObject();
    completeChannnelDataPack(data_pack.get(), false);
    //send pack
    return client_instance()->sendServiceData(ChaosMoveOperator(data_pack));
}

int DirectIOVirtualClientChannel::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                  chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //set the endpoint that need the receive the pack on the other side
    //data_pack->header.dispatcher_header.fields.route_addr = endpoint;
    //set the channel route index within the endpoint
    data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
    
    //convert default DirectIO hader to little endian
    DIRECT_IO_DATAPACK_FROM_ENDIAN(data_pack)
    DirectIOForwarderPtrLOWriteLock wl = client_instance.getWriteLockObject();
    completeChannnelDataPack(data_pack.get(), true);
    //send pack
    int err = client_instance()->sendServiceData(ChaosMoveOperator(data_pack),
                                                 synchronous_answer);
    if(!err){
        if(synchronous_answer &&
           synchronous_answer->channel_data) {
            //convert default DirectIO hader to little endian
            DIRECT_IO_DATAPACK_TO_ENDIAN(synchronous_answer)
            //report api error as function error
            err = synchronous_answer->header.dispatcher_header.fields.err;
        }
    }
    return err;
}

//! default header deallocator implementation
void DirectIOVirtualClientChannel::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
    delete(sent_data_ptr);
}
