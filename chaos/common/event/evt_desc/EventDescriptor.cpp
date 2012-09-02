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
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/utility/endianess.h>

using namespace chaos;
using namespace chaos::event;

#define EVENT_CURRENT_VERSION 0

    //--------------------------------------------------------------
EventDescriptor::EventDescriptor(EventType _instanceType, uint8_t _instancePriority):instanceType(_instanceType), instancePriority(_instancePriority) {
        //initialize data
    initData();
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
    

        //use a temp ptr to go forward the buffer
    tmpPtr = eventData + EVT_HEADER_BYTE_LENGTH;
    
    EventTypeAndPriority typeAndPriority = {instanceType, instancePriority};
    *((uint8_t*)tmpPtr) = byte_swap<host_endian, little_endian, uint8_t>(*((uint8_t*)&typeAndPriority));
}

    //--------------------------------------------------------------
EventDescriptor::~EventDescriptor() {
        //if(eventData != NULL)  delete[] eventData;
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

    //!Create a new event from serialized data
/*
 Initializze all information to manage an event type. Can also create
 a event constructor by already instatiated memory
 \param serializedEvent pointer to an event data, the buff this is length 1024 byte
 */
void EventDescriptor::setEventData(const unsigned char *serializedEvent, uint16_t length) {
        //co
    memcpy(eventData, serializedEvent, length);
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

    //!Set the code of the alert
/*
 Set the allert code for this event
 \param alertCode thecode of the alert
 */
void EventDescriptor::setSubCode(uint16_t subCode) {
    *((uint16_t*)(eventData+EVT_SUB_CODE_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(subCode);
}


    //!Return the code of this alert
/*
 Return the alert code identified bythis event
 \return the code of the alert
 */
uint16_t EventDescriptor::getSubCode(){
    return static_cast<uint16_t>(byte_swap<little_endian, host_endian, uint16_t>( *((uint16_t*)(eventData+EVT_SUB_CODE_OFFSET))));
}

    //!Set the priority of this alert
/*
 Set the allert priority
 \param alertPriority priority
 */
void EventDescriptor::setSubCodePriority(uint16_t subCodePriority){
    *((uint16_t*)(eventData + EVT_SUB_CODE_PRIORITY_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(subCodePriority);
}

    //!Return the code of this alert
/*
 Return the alert code identified bythis event
 \return the code of the alert
 */
uint16_t EventDescriptor::getSubCodePriority() {
    return byte_swap<little_endian, host_endian, uint16_t>( *((uint16_t*)(eventData + EVT_SUB_CODE_PRIORITY_OFFSET)));
    
}


uint8_t EventDescriptor::getIdentificationlength() {
    return *((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET));
}

uint16_t EventDescriptor::setSenderIdentification(const char * const identification, uint8_t identificationLength) {
    *((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET)) = identificationLength;
    if(identificationLength > 0 && identification){
            //write the identifier
        memcpy((void*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET), identification, identificationLength);
    }
    return identificationLength;
}


const char * const EventDescriptor::getIdentification() {
    if(!*((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET))) return NULL;
    return (const char *)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET);
}

void EventDescriptor::setIdentificationAndValueWithType(const char * identification, uint8_t identificationLength, EventDataType valueType, const void *valuePtr, uint16_t valueSize) {
    uint16_t dataDim = 0;
    uint16_t indetificationSize = setSenderIdentification(identification, identificationLength);

    
    *((uint8_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize)) = (uint8_t)valueType;
    
    switch (valueType) {
        case EventDataInt8:
            dataDim = sizeof(uint8_t);
            *((uint8_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1)) = byte_swap<host_endian, little_endian, uint8_t>(*((uint8_t*)valuePtr));
            break;
        case  EventDataInt16:
            dataDim = sizeof(uint16_t);
            *((uint16_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1)) = byte_swap<host_endian, little_endian, uint16_t>(*((uint16_t*)valuePtr));
            break;
        case  EventDataInt32:
            dataDim = sizeof(uint32_t);
            *((uint32_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1)) = byte_swap<host_endian, little_endian, uint32_t>(*((uint32_t*)valuePtr));
            break;
        case  EventDataInt64:
            dataDim = sizeof(uint64_t);
            *((uint64_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1)) = byte_swap<host_endian, little_endian, uint64_t>(*((uint64_t*)valuePtr));
            break;
        case  EventDataDouble:
            dataDim = sizeof(double);
            *((double*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1)) = byte_swap<host_endian, little_endian, double>(*((double*)valuePtr));
            break;
        case   EventDataCString:
        case    EventDataBinary:
            dataDim = valueSize;
            memcpy((eventData +  EVT_IDENTIFICATION_VALUE_INFO_OFFSET + indetificationSize + 1), valuePtr, valueSize);
            break;
    }
    
        //set ehe new data length
    setEventDataLength(EVT_IDENTIFICATION_VALUE_INFO_OFFSET + dataDim + indetificationSize + 1);
    return ;
}

EventDataType EventDescriptor::getEventValueType() {
    uint8_t idLen = *((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET));
    int type = *((uint8_t*)(eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET + idLen));
    return (EventDataType)type;
}


uint16_t EventDescriptor::getEventValueSize() {
    uint8_t idLen = *((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET));
    uint16_t packDimension = getEventDataLength();
    return packDimension - (EVT_IDENTIFICATION_LENGTH_INFO_OFFSET + idLen + 2);
}

void EventDescriptor::getEventValue(void *valuePtr, uint16_t *size) {
    if(valuePtr != NULL && !size) return;

    uint8_t idLen = *((uint8_t*)(eventData + EVT_IDENTIFICATION_LENGTH_INFO_OFFSET));
    uint16_t packDimension = getEventDataLength();
    *size =  packDimension - (EVT_IDENTIFICATION_LENGTH_INFO_OFFSET + idLen + 2);
    
    memcpy(valuePtr, (eventData + EVT_IDENTIFICATION_VALUE_INFO_OFFSET+idLen+1), *size);
}

