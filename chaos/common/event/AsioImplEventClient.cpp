/*
 *	AsioImplEventClient.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 25/08/12.
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

#include <chaos/common/event/AsioImplEventClient.h>
#include <chaos/common/event/AsioEventForwarder.h>
#include <chaos/common/chaos_constants.h>

using namespace chaos;
using namespace chaos::common::event;

#define CREATE_EVENT_SOCKET(s, addr, port, err, msg)\
s = new boost::asio::ip::udp::socket(io_service);\
if(s == NULL) {\
throw CException(err, msg, __PRETTY_FUNCTION__);\
} else {\
s->connect(boost::asio::ip::udp::endpoint(addr,port));\
}\

#define DELETE_EVENT_SOCKET(s)\
if(s){s->close(); delete(s);}

#define SEND_EVENT_DATA(s, e)\
s->send(boost::asio::buffer(e->getEventData(), e->getEventDataLength()));

DEFINE_CLASS_FACTORY(AsioImplEventClient, EventClient);
AsioImplEventClient::AsioImplEventClient(string alias):
EventClient(alias),
socket_alert(NULL),
socket_instrument(NULL),
socket_command(NULL),
socket_custom(NULL) {
    threadNumber = 0;
}

AsioImplEventClient::~AsioImplEventClient() {
    
}

/*
 init the event adapter
 */
void AsioImplEventClient::init(void* initParameter) throw(CException) {
    threadNumber = 1;
    //alertForwrder
    CREATE_EVENT_SOCKET(socket_alert,
                        asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_ALERT_MADDRESS),
                        event::EventConfiguration::CONF_EVENT_PORT,
                        -1,
                        "Error on creating alert socket");
    
    CREATE_EVENT_SOCKET(socket_instrument,
                        asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_INSTRUMENT_MADDRESS),
                        event::EventConfiguration::CONF_EVENT_PORT,
                        -2,
                        "Error on creating instrument socket");
    CREATE_EVENT_SOCKET(socket_command,
                        asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_COMMAND_MADDRESS),
                        event::EventConfiguration::CONF_EVENT_PORT,
                        -3,
                        "Error on creating command socket");
    CREATE_EVENT_SOCKET(socket_custom,
                        asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_CUSTOM_MADDRESS),
                        event::EventConfiguration::CONF_EVENT_PORT,
                        -4,
                        "Error on creating custom socket");
    
    CObjectProcessingPriorityQueue<EventDescriptor>::init(1);
}

/*
 start the event adapter
 */
void AsioImplEventClient::start() throw(CException) {
    
    
    //register forall event
    //create the services
    
    for (int idx = 0; idx < threadNumber; idx++) {
        //create the handler
        // ChaosSharedPtr<thread> thread();
        service_thread_group.add_thread(new boost::thread(bind(&asio::io_service::run, &io_service)));
    }
}

//-----------------------
void AsioImplEventClient::stop() throw(CException) {
    
}

/*
 deinit the event adapter
 */
void AsioImplEventClient::deinit() throw(CException) {
    DELETE_EVENT_SOCKET(socket_alert);
    DELETE_EVENT_SOCKET(socket_instrument);
    DELETE_EVENT_SOCKET(socket_command);
    DELETE_EVENT_SOCKET(socket_custom);
    
    CObjectProcessingPriorityQueue<EventDescriptor>::clear();
    CObjectProcessingPriorityQueue<EventDescriptor>::deinit();
    
    io_service.stop();
    
    // Wait for all threads in the pool to exit.
    service_thread_group.join_all();
}

void AsioImplEventClient::processBufferElement(EventDescriptor *event,
                                               ElementManagingPolicy& policy) throw(CException) {
        switch (event->getEventType()) {
            case event::EventTypeAlert:
                SEND_EVENT_DATA(socket_alert, event);
                break;
            case event::EventTypeInstrument:
                SEND_EVENT_DATA(socket_instrument, event);
                break;
            case event::EventTypeCommand:
                SEND_EVENT_DATA(socket_command, event);
                break;
            case event::EventTypeCustom:
                SEND_EVENT_DATA(socket_custom, event);
                break;
        }
    
}

//! abstract queue action method implementation
bool AsioImplEventClient::submitEvent(EventDescriptor *event)  throw(CException) {
    return CObjectProcessingPriorityQueue<EventDescriptor>::push(event,
                                                                 event->getEventPriority());
}
