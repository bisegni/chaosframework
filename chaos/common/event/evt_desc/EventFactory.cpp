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

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/event/evt_desc/EventFactory.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>

using namespace chaos;
using namespace chaos::common::event;
using namespace chaos::common::event::alert;
using namespace chaos::common::event::instrument;
using namespace chaos::common::event::command;
using namespace chaos::common::event::custom;
using namespace chaos::common::utility;

EventDescriptor *EventFactory::getEventInstance(const unsigned char * serializedEvent,  uint16_t length)  throw (CException) {
    EventDescriptor *result = NULL;
    if(serializedEvent == NULL) throw CException(0, "Invalid pointer to serialized event", "EventFactory::getEventInstance");
    if(length > EVT_DATA_MAX_BYTE_LENGTH)  throw CException(1, "Event memory size exceed the max allowed", "EventFactory::getEventInstance");
    if(*((uint16_t*)serializedEvent) != 0x4345) throw CException(2, "Bad event signature", "EventFactory::getEventInstance");
    
        //check the type
	ChaosUniquePtr<EventTypeAndPriority> eventTypeAndHeaderPtr(new EventTypeAndPriority);
        
        //get header swapped checking endian conversion
    *((uint16_t*)eventTypeAndHeaderPtr.get()) = byte_swap<little_endian, host_endian, uint16_t>(*((uint16_t*)(serializedEvent+EVT_HEADER_BYTE_LENGTH)));
   
    switch (eventTypeAndHeaderPtr->type) {
        case 0: // EVT_TYPE_CODE==0
            result = new event::alert::AlertEventDescriptor();
            break;
        default:
                throw CException(3, "Event not yet implemented", "EventFactory::getEventInstance");
            break;
    }
    result->setEventData(serializedEvent, length);
    return result;
}
