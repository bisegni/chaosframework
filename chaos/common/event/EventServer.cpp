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
#include <chaos/common/global.h>
#include <chaos/common/event/EventServer.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::event;
using namespace chaos::common::utility;

EventServer::EventServer(const string& alias):
NamedService(alias),
rootEventHandler(NULL){
}

void EventServer::setEventHanlder(EventHandler *newEventHanlder) {
    rootEventHandler = newEventHanlder;
}

void EventServer::dispatchEventToHandler(const unsigned char * const serializedEvent, uint16_t length)  {
    CHAOS_ASSERT(rootEventHandler && serializedEvent && length);
    try{
        if(*((uint16_t*)serializedEvent) != 0x4345) throw CException(2, "Bad event signature", "EventServer::dispatchEventToHandler");
        if(length > EVT_DATA_MAX_BYTE_LENGTH)  throw CException(1, "Event memory size exceed the max allowed", "EventServer::dispatchEventToHandler");
        
        //check the type
        ChaosUniquePtr<EventTypeAndPriority> eventTypeAndHeaderPtr(new EventTypeAndPriority());
        
        //get header swapped checking endian conversion
        *((uint8_t*)eventTypeAndHeaderPtr.get()) = byte_swap<little_endian, host_endian, uint8_t>(*((uint8_t*)(serializedEvent+EVT_HEADER_BYTE_LENGTH)));
        
        //dispatcher the event in the root handler on one of the fourth method
        switch (eventTypeAndHeaderPtr->type) {
            case EventTypeAlert:  {
                alert::AlertEventDescriptor *result = new alert::AlertEventDescriptor();
                result->setEventData(serializedEvent, length);
                rootEventHandler->executeAlertHandler(result);
            }
                break;
                
            case EventTypeInstrument:  {
                instrument::InstrumentEventDescriptor *result = new instrument::InstrumentEventDescriptor();
                result->setEventData(serializedEvent, length);
                rootEventHandler->executeInstrumentHandler(result);
            }
                break;
            default:
                throw CException(3, "Event not yet implemented", "EventFactory::getEventInstance");
                break;
        }
    } catch(CException &ex){
        DECODE_CHAOS_EXCEPTION(ex);
    }
    
}
