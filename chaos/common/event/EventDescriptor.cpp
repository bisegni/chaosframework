/*
 *	EventDescriptor.cpp
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
#include <string.h>
#include <chaos/common/event/EventDescriptor.h>
#include <chaos/common/utility/endianess.h>

using namespace chaos;

#define EVENT_CURRENT_VERSION 0

    //--------------------------------------------------------------
EventDescriptor::EventDescriptor() {
    eventData = new unsigned char[EVENT_DATA_BYTE_LENGTH];
}

    //--------------------------------------------------------------
void EventDescriptor::initData() {
        //clear all data
    memset(eventData, 0, EVENT_DATA_BYTE_LENGTH);
    
        //use a temp ptr to go forward the buffer
    unsigned char * tmpPtr = eventData;

        //header section -----
        //event header struct
    EventHeader header = {0x4345, 1, 5};
    *((uint32_t*)tmpPtr) = byte_swap<host_endian, little_endian, uint32_t>(*((uint32_t*)&header));
 }

    //--------------------------------------------------------------
EventDescriptor::~EventDescriptor() {
    if(eventData != NULL)  delete[] eventData;
}

    //--------------------------------------------------------------
uint8_t EventDescriptor::getEventHeaderVersion() {
    uint32_t tmp32 = byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)eventData));
    return static_cast<uint8_t>(((EventHeader*)&tmp32)->version);
}

    //--------------------------------------------------------------
void EventDescriptor::setEventDataLength(uint8_t newSize) {
    uint32_t tmp32 = byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)eventData));
        //get header converting endianess
    ((EventHeader*)&tmp32)->length = newSize;
    *((uint32_t*)eventData) = byte_swap<host_endian, little_endian, uint32_t>(tmp32);
}

    //--------------------------------------------------------------
uint8_t EventDescriptor::getEventDataLength() {
    uint32_t tmp32 = byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)eventData));
    return static_cast<uint8_t>(((EventHeader*)&tmp32)->length);
}

    //--------------------------------------------------------------
const unsigned char * const EventDescriptor::getEventData() {
    return eventData;
}

    //--------------------------------------------------------------
uint8_t EventDescriptor::getEventType() {
    uint8_t tmp8 = byte_swap<little_endian, host_endian, uint8_t>(*((uint8_t*)(eventData+EVT_HEADER_BYTE_LENGTH)));
    return ((EventTypeAndPriority*)&tmp8)->type;
}

    //--------------------------------------------------------------
uint8_t EventDescriptor::getEventPriority() {
    uint8_t tmp8 = byte_swap<little_endian, host_endian, uint8_t>(*((uint8_t*)(eventData+EVT_HEADER_BYTE_LENGTH)));
    return ((EventTypeAndPriority*)&tmp8)->priority;
}