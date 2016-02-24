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
using namespace chaos::common::event;
using namespace chaos::common::event::instrument;

InstrumentEventDescriptor::InstrumentEventDescriptor():EventDescriptor(EventTypeInstrument, EVT_INSTRUMENT_DEFAULT_PRIORITY) {
    
}

    //---------
void InstrumentEventDescriptor::setNewScheduleDelay(const std::string& instrumentID, uint64_t newValue) {
        //2 byte
    setSubCode(EventInstrumentNewScheduleDelay);
        //2 byte
    setSubCodePriority(0);
        //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(instrumentID,
                                                       EventDataInt64,
                                                       &newValue);
}



void InstrumentEventDescriptor::setDatasetInputAttributeChanged(const std::string&  instrumentID,
                                                                uint16_t errorCode) {
    //2 byte
    setSubCode(EventInstrumentInputDatasetAttributeChanged);
    //2 byte
    setSubCodePriority(0);
    //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(instrumentID,
                                                       EventDataInt16,
                                                       &errorCode);
  
}

//! Define the event for the heartbeat of the instrument
void InstrumentEventDescriptor::setEartbeat(const std::string&  instrumentID) {
    uint8_t state = 0;
    //2 byte
    setSubCode(EventInstrumentHeartbeat);
    //2 byte
    setSubCodePriority(0);
    //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(instrumentID,
                                                       EventDataInt8,
                                                       &state);
}

void InstrumentEventDescriptor::setInstrument(const std::string& indetifier,
                                              uint16_t alert_code,
                                              uint16_t priority,
                                              EventDataType value_type,
                                              const void *value_ptr,
                                              uint16_t value_size)  throw (CException) {
    if(alert_code <= EventAlertLastCodeNumber) throw CException(0, "Invalid custom sub code for the event", "InstrumentEventDescriptor::setInstrument");
        //2 byte
    setSubCode(alert_code);
        //2 byte
    setSubCodePriority(priority);
        //set the dimension, 10 is the fixed size of all information for alert pack
    EventDescriptor::setIdentificationAndValueWithType(indetifier,
                                                       value_type,
                                                       value_ptr,
                                                       value_size);
}