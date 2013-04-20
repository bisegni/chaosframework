/*
 *	EventServer.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/event/EventServer.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>

using namespace chaos;
using namespace chaos::event;

EventServer::EventServer(string *alias):NamedService(alias){
}

void EventServer::setEventHanlder(EventHandler *newEventHanlder) {
    rootEventHandler = newEventHanlder;
}

void EventServer::dispatchEventToHandler(const unsigned char * const serializedEvent, uint16_t length) throw (CException) {
    CHAOS_ASSERT(rootEventHandler && serializedEvent && length);
    try{
        if(*((uint16_t*)serializedEvent) != 0x4345) throw CException(2, "Bad event signature", "EventServer::dispatchEventToHandler");
        if(length > EVT_DATA_MAX_BYTE_LENGTH)  throw CException(1, "Event memory size exceed the max allowed", "EventServer::dispatchEventToHandler");
        
            //check the type
        auto_ptr<EventTypeAndPriority> eventTypeAndHeaderPtr(new EventTypeAndPriority());
        
            //get header swapped checking endian conversion
        *((uint16_t*)eventTypeAndHeaderPtr.get()) = byte_swap<little_endian, host_endian, uint16_t>(*((uint16_t*)(serializedEvent+EVT_HEADER_BYTE_LENGTH)));
        
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