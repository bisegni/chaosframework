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

#include <cstring>
#include <chaos/common/direct_io/DirectIODispatcher.h>
#include <chaos/common/global.h>
using namespace chaos::common::direct_io;

#define DirectIODispatcher_LOG_HEAD "[DirectIODispatcher] - "

#define DIOD_LAPP_ LAPP_ << DirectIODispatcher_LOG_HEAD
#define DIOD_LDBG_ LDBG_ << DirectIODispatcher_LOG_HEAD
#define DIOD_LERR_ LERR_ << DirectIODispatcher_LOG_HEAD

#define MAX_ENDPOINT_NUMBER 256
#define MAX_ENDPOINT_ARRAY_SIZE (sizeof(EndpointFastDelegation*) * MAX_ENDPOINT_NUMBER)

#define CLEAR_ENDPOINT_SLOT(i) \
if(endpoint_slot_array[i]->endpoint) { \
delete endpoint_slot_array[i]->endpoint; \
endpoint_slot_array[i]->endpoint = NULL; \
} \
endpoint_slot_array[i]->enable = false;


DirectIODispatcher::DirectIODispatcher():
available_endpoint_slot(MAX_ENDPOINT_NUMBER),
endpoint_slot_array(NULL) {}

DirectIODispatcher::~DirectIODispatcher(){
    CHAOS_NOT_THROW(stop();)
    CHAOS_NOT_THROW(deinit();)
}

// Initialize instance
void DirectIODispatcher::init(void *init_data) throw(chaos::CException) {
    //allocate memory for the endpoint array
    if(endpoint_slot_array==NULL){
        DIOD_LDBG_<< "Allocating all memory for endpoint slot array";
        
        endpoint_slot_array = (EndpointFastDelegation**)malloc(MAX_ENDPOINT_NUMBER*sizeof(EndpointFastDelegation*));
        if(!endpoint_slot_array)  throw chaos::CException(-1, "Error allocating memory for slot endpoint", __FUNCTION__);
        
        
        DIOD_LDBG_ << "Allocating all endpoint slot";
        //allocate all endpoint slot
        for (int idx = 0; idx < MAX_ENDPOINT_NUMBER; idx++) {
            endpoint_slot_array[idx] = new EndpointFastDelegation();
            endpoint_slot_array[idx]->enable = false;
            endpoint_slot_array[idx]->endpoint = NULL;
            
            //add this endpoint to free slot queue
            available_endpoint_slot.push(idx);
        }
    }
}

// Start the implementation
void DirectIODispatcher::start() throw(chaos::CException) {}

// Stop the implementation
void DirectIODispatcher::stop() throw(chaos::CException) {}

// Deinit the implementation
void DirectIODispatcher::deinit() throw(chaos::CException) {
    if(endpoint_slot_array) {
        DIOD_LDBG_ << "Deallocating all endpoint slot";
        //allocate all endpoint slot
        for (int idx = 0; idx < MAX_ENDPOINT_NUMBER; idx++) {
            
            //clear all field of the slod ( and delete the endpoint if allocated)
            CLEAR_ENDPOINT_SLOT(idx);
            
            //delete andpoint slot
            delete endpoint_slot_array[idx];
        }
        
        DIOD_LDBG_ << "Free slot array memory";
        free(endpoint_slot_array);
        endpoint_slot_array=NULL;
    }
    unsigned int tmp;
    while(!available_endpoint_slot.empty()){available_endpoint_slot.pop(tmp);}
}

//allocate new endpoint
DirectIOServerEndpoint *DirectIODispatcher::getNewEndpoint() {
    unsigned int next_available_slot = -1;
    if(!available_endpoint_slot.pop(next_available_slot)) return NULL;
    ChaosWriteLock wl(slot_mutex);
    
    endpoint_slot_array[next_available_slot]->endpoint = new DirectIOServerEndpoint();
    if(!endpoint_slot_array[next_available_slot]->endpoint) {
        //reallocate the slot index
        available_endpoint_slot.push(next_available_slot);
        return NULL;
    }
    //associate to the endpoint his slot index
    endpoint_slot_array[next_available_slot]->endpoint->endpoint_route_index = next_available_slot;
    endpoint_slot_array[next_available_slot]->endpoint->server_public_interface = server_public_interface;
    endpoint_slot_array[next_available_slot]->enable = true;
    return endpoint_slot_array[next_available_slot]->endpoint;
}

//! relase an endpoint
void DirectIODispatcher::releaseEndpoint(DirectIOServerEndpoint *endpoint_to_release) {
    if(!endpoint_to_release) return;
    ChaosWriteLock wl(slot_mutex);
    // get slot index
    unsigned int slot_idx = endpoint_to_release->endpoint_route_index;
    
    //delete endpoint
    CLEAR_ENDPOINT_SLOT(slot_idx);
    
    //reuse the index
    available_endpoint_slot.push(slot_idx);
}

// Event for a new data received
int DirectIODispatcher::priorityDataReceived(DirectIODataPackSPtr data_pack,
                                             DirectIODataPackSPtr& synchronous_answer) {
    int api_err = 0;
    if(getServiceState() != 2) return -1;
    CHAOS_ASSERT(data_pack.get());
    ChaosReadLock wl(slot_mutex);

    uint8_t     opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    uint16_t    tmp_addr = data_pack->header.dispatcher_header.fields.route_addr;
    uint16_t    message_counter = data_pack->header.dispatcher_header.fields.counter;
    //convert dispatch header to correct endianes
    DIRECT_IO_DATAPACK_DISPATCH_HEADER_FROM_ENDIAN(data_pack.get());
    
    CHAOS_ASSERT(tmp_addr == data_pack->header.dispatcher_header.fields.route_addr);
    if(data_pack->header.dispatcher_header.fields.route_addr>=MAX_ENDPOINT_NUMBER){
        DIOD_LERR_ << "The endpoint address " << data_pack->header.dispatcher_header.fields.route_addr << "is invalid";
    } else if(endpoint_slot_array[data_pack->header.dispatcher_header.fields.route_addr]->enable) {
        api_err = endpoint_slot_array[data_pack->header.dispatcher_header.fields.route_addr]->endpoint->priorityDataReceived(MOVE(data_pack),
                                                                                                                             synchronous_answer);
    } else {
        DIOD_LERR_ << "The endpoint address " << tmp_addr << "is disable";
        return -2;
    }
    if(synchronous_answer.get()) {
        //set opcode for the answer
        synchronous_answer->header.dispatcher_header.fields.channel_opcode = opcode;
        //set counter for the answer
        synchronous_answer->header.dispatcher_header.fields.counter = message_counter;
        //set error on result datapack
        synchronous_answer->header.dispatcher_header.fields.err = (int16_t)api_err;
        //convert dispatch header to correct endianes
        DIRECT_IO_DATAPACK_DISPATCH_HEADER_TO_ENDIAN(synchronous_answer.get());
    }
    return 0;
}

// Event for a new data received
int DirectIODispatcher::serviceDataReceived(DirectIODataPackSPtr data_pack,
                                            DirectIODataPackSPtr& synchronous_answer) {
    int api_err = 0;
    if(getServiceState() != 2) return -1;
    CHAOS_ASSERT(data_pack.get());
    ChaosReadLock wl(slot_mutex);
    
    uint8_t     opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    uint16_t    tmp_addr = data_pack->header.dispatcher_header.fields.route_addr;
    uint16_t    message_counter = data_pack->header.dispatcher_header.fields.counter;
    //convert dispatch header to correct endianes
    DIRECT_IO_DATAPACK_DISPATCH_HEADER_FROM_ENDIAN(data_pack.get());
    
    CHAOS_ASSERT(tmp_addr == data_pack->header.dispatcher_header.fields.route_addr);
    
    if(data_pack->header.dispatcher_header.fields.route_addr>=MAX_ENDPOINT_NUMBER){
        DIOD_LERR_ << "The endpoint address " << data_pack->header.dispatcher_header.fields.route_addr << "is invalid";
    } else if(endpoint_slot_array[data_pack->header.dispatcher_header.fields.route_addr]->enable) {
        api_err = endpoint_slot_array[data_pack->header.dispatcher_header.fields.route_addr]->endpoint->serviceDataReceived(MOVE(data_pack),
                                                                                                                            synchronous_answer);
    } else {
        DIOD_LERR_ << "The endpoint address " << tmp_addr << "is disable";
        return -2;
    }
    if(synchronous_answer.get()) {
        //set opcode for the answer
        synchronous_answer->header.dispatcher_header.fields.channel_opcode = opcode;
        //set counter for the answer
        synchronous_answer->header.dispatcher_header.fields.counter = message_counter;
        //set error on result datapack
        synchronous_answer->header.dispatcher_header.fields.err = (int16_t)api_err;
        //convert dispatch header to correct endianes
        DIRECT_IO_DATAPACK_DISPATCH_HEADER_TO_ENDIAN(synchronous_answer.get());
    }
    return 0;
}
