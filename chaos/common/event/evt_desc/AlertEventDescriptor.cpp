/*
 *	AlertEventDescriptor.cpp
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
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>

using namespace chaos;
using namespace chaos::event;
using namespace chaos::event::alert;

    //!Event data inizialization
/*
 Initialize the data for the base descriptor competence. These are
 only the header and base length (header only length)
 */
void AlertEventDescriptor::initData() {
        //call base data
    EventDescriptor::initData();
        //use a temp ptr to go forward the buffer
    unsigned char * tmpPtr = eventData + EVT_HEADER_BYTE_LENGTH;
    
    EventTypeAndPriority typeAndPriority = {EVT_TYPE_CODE, EVT_ALERT_DEFAULT_PRIORITY};
    *((uint8_t*)tmpPtr) = byte_swap<host_endian, little_endian, uint8_t>(*((uint8_t*)&typeAndPriority));
}


    //!Set the code of the alert
/*
 Set the allert code for this event
 \param alertCode thecode of the alert
 */
void AlertEventDescriptor::setAlertCode(EventAlertCode alertCode) {
    *((uint16_t*)(eventData+EVT_ALERT_CODE_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(alertCode);
}

    //!Return the code of this alert
/*
 Return the alert code identified bythis event
 \return the code of the alert
 */
EventAlertCode AlertEventDescriptor::getAlertCode(){
    uint16_t resultCode = byte_swap<little_endian, host_endian, uint16_t>( *((uint16_t*)(eventData+EVT_ALERT_CODE_OFFSET)));
    return static_cast<EventAlertCode>(resultCode);
}

    //!Set the code of the alert
/*
 Set the allert code for this event
 \param alertCode thecode of the alert
 */
void AlertEventDescriptor::setAlertCustomCode(uint16_t alertCustomCode) {
    *((uint16_t*)(eventData+EVT_ALERT_CODE_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(alertCustomCode + EventAlertLastCodeNumber);
}

    //!Return the code of this alert
/*
 Return the alert code identified bythis event
 \return the code of the alert
 */
uint16_t AlertEventDescriptor::getAlertCustomCode(){
    return static_cast<EventAlertCode>(byte_swap<little_endian, host_endian, uint16_t>( *((uint16_t*)(eventData+EVT_ALERT_CODE_OFFSET)))-EventAlertLastCodeNumber);
}

    //!Set the priority of this alert
/*
 Set the allert priority
 \param alertPriority priority
 */
void AlertEventDescriptor::setAlertPriority(uint16_t alertPriority){
    *((uint16_t*)(eventData + EVT_ALERT_PRORITY_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(alertPriority);
}

    //!Return the code of this alert
/*
 Return the alert code identified bythis event
 \return the code of the alert
 */
uint16_t AlertEventDescriptor::getAlertPriority() {
    return byte_swap<little_endian, host_endian, uint16_t>( *((uint16_t*)(eventData + EVT_ALERT_PRORITY_OFFSET)));
    
}

EventDataType AlertEventDescriptor::getValuesType() {
    return *((EventDataType*)(eventData + EVT_ALERT_DATA_TYPE_OFFSET));
}

uint16_t AlertEventDescriptor::setValueWithType(EventDataType valueType, const void *valuePtr, uint16_t valueSize) {
    uint16_t dataDim = 0;
    switch (valueType) {
        case EventDataInt8:
            dataDim = sizeof(uint8_t);
            *((uint8_t*)(eventData + EVT_ALERT_DATA_OFFSET)) = byte_swap<host_endian, little_endian, uint8_t>(*((uint8_t*)valuePtr));
            break;
        case  EventDataInt16:
            dataDim = sizeof(uint16_t);
            *((uint16_t*)(eventData + EVT_ALERT_DATA_OFFSET)) = byte_swap<host_endian, little_endian, uint16_t>(*((uint16_t*)valuePtr));
            break;
        case  EventDataInt32:
            dataDim = sizeof(uint32_t);
            *((uint32_t*)(eventData + EVT_ALERT_DATA_OFFSET)) = byte_swap<host_endian, little_endian, uint32_t>(*((uint32_t*)valuePtr));
            break;
        case  EventDataInt64:
            dataDim = sizeof(uint64_t);
            *((uint64_t*)(eventData + EVT_ALERT_DATA_OFFSET)) = byte_swap<host_endian, little_endian, uint64_t>(*((uint64_t*)valuePtr));
            break;
        case  EventDataDouble:
            dataDim = sizeof(double);
            *((double*)(eventData + EVT_ALERT_DATA_OFFSET)) = byte_swap<host_endian, little_endian, double>(*((double*)valuePtr));
            break;
        case   EventDataCString:
        case    EventDataBinary:
            dataDim = valueSize;
            memcpy((eventData + EVT_ALERT_DATA_OFFSET), valuePtr, valueSize);
            break;
    }
    return dataDim;
 }

/*!
 Set the Value for the type
 \param valueType the enumeration that descrive the type of the value
 \param valuePtr a pointer to the value
 \param valueSizethe size of the value
 */
void AlertEventDescriptor::setAlert(EventAlertCode alertCode, uint16_t priority, EventDataType valueType, const void *valuePtr, uint16_t valueSize) {
        //2 byte
    setAlertCode(alertCode);
        //2 byte
    setAlertPriority(priority);
        //set the dimension, 10 is the fixed size of all information for alert pack
    setEventDataLength(setValueWithType(valueType, valuePtr, valueSize) +  EVT_ALERT_DATA_OFFSET);
}

/*!
 Set the Value for the custom type
 \param valueType the enumeration that descrive the type of the value with EventDataType constant
 \param valuePtr a pointer to the value
 \param valueSizethe size of the value
 */
void AlertEventDescriptor::setCustomAlert(uint16_t alertCustomCode, uint16_t priority, EventDataType valueType, const void *valuePtr, uint16_t valueSize) {
        //2 byte
    setAlertCustomCode(alertCustomCode);
        //2 byte
    setAlertPriority(priority);
        //set the dimension, 10 is the fixed size of all information for alert pack
    setEventDataLength(setValueWithType(valueType, valuePtr, valueSize) +  EVT_ALERT_DATA_OFFSET);
}

uint16_t AlertEventDescriptor::getAlertValueSize() {
    uint16_t packDimension = getEventDataLength();
    return packDimension - EVT_ALERT_DATA_OFFSET;
}

void AlertEventDescriptor::getAlertValue(void *valuePtr, uint16_t valueSize) {
    if(valuePtr != NULL || !valueSize) return;
    memcpy(valuePtr, (eventData + EVT_ALERT_DATA_OFFSET), valueSize);
}
