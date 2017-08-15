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