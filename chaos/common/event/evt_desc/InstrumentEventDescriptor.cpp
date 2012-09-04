/*
 *	InstrumentEventDescriptor.cpp
 *	!CHAOS
 *	Created by Claudio Bisegni on 19/08/12.
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

/*
 Intellectual property of 2012 INFN.
 */
#include "InstrumentEventDescriptor.h"

using namespace chaos;
using namespace chaos::event;
using namespace chaos::event::instrument;

InstrumentEventDescriptor::InstrumentEventDescriptor():EventDescriptor(EventTypeInstrument, EVT_INSTRUMENT_DEFAULT_PRIORITY) {
    
}

    //---------
void InstrumentEventDescriptor::setNewScheduleDelay(const char * const deviceID, uint64_t newValue) {
        //2 byte
    setSubCode(EventInstrumentNewScheduleDelay);
        //2 byte
    setSubCodePriority(0);
        //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(deviceID, strlen(deviceID), EventDataInt64, &newValue);
}


void InstrumentEventDescriptor::setInstrument(const char * const indetifier, uint8_t identifierLength, uint16_t alertCode, uint16_t priority, EventDataType valueType, const void *valuePtr, uint16_t valueSize)  throw (CException) {
    if(alertCode <= EventAlertLastCodeNumber) throw CException(0, "Invalid custom sub code for the event", "InstrumentEventDescriptor::setInstrument");
        //2 byte
    setSubCode(alertCode);
        //2 byte
    setSubCodePriority(priority);
        //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(indetifier, identifierLength, valueType, valuePtr, valueSize);
}