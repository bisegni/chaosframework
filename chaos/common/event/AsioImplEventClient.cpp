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
#include <chaos/common/cconstants.h>

using namespace chaos;
using namespace chaos::event;

AsioImplEventClient::AsioImplEventClient(string *alias):EventClient(alias) {
    threadNumber = 0;
    alertForwarder = NULL;
    instrumentForwarder = NULL;
    commandForwarder = NULL;
    customForwarder = NULL;
}

AsioImplEventClient::~AsioImplEventClient() {
    
}

/*
 init the event adapter
 */
void AsioImplEventClient::init(void* initParameter) throw(CException) {
    threadNumber = 4;
        //alertForwrder
    alertForwarder = new AsioEventForwarder(asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_ALERT_MADDRESS),
                                            event::EventConfiguration::CONF_EVENT_PORT,
                                            io_service);
    alertForwarder->init();
    
    instrumentForwarder = new AsioEventForwarder(asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_INSTRUMENT_MADDRESS),
                                            event::EventConfiguration::CONF_EVENT_PORT,
                                            io_service);
    instrumentForwarder->init();
    
    commandForwarder = new AsioEventForwarder(asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_COMMAND_MADDRESS),
                                            event::EventConfiguration::CONF_EVENT_PORT,
                                            io_service);
    commandForwarder->init();
    
    customForwarder = new AsioEventForwarder(asio::ip::address::from_string(event::EventConfiguration::CONF_EVENT_CUSTOM_MADDRESS),
                                            event::EventConfiguration::CONF_EVENT_PORT,
                                            io_service);
    customForwarder->init();
}

/*
 start the event adapter
 */
void AsioImplEventClient::start() throw(CException) {
    
        //register forall event
        //create the services
    
    for (int idx = 0; idx < threadNumber; idx++) {
            //create the handler
        shared_ptr<thread> thread(new boost::thread(bind(&asio::io_service::run, &io_service)));
        serviceThread.push_back(thread);
    }
}

//-----------------------
void AsioImplEventClient::stop() throw(CException) {
    
}

/*
 deinit the event adapter
 */
void AsioImplEventClient::deinit() throw(CException) {
    try{
        if(alertForwarder) {
           alertForwarder->deinit();
            delete (alertForwarder);
        }
    }catch(...){
        
    }
    try{
        if(instrumentForwarder) {
            instrumentForwarder->deinit();
            delete(instrumentForwarder);
        }
    }catch(...){
        
    }
    try{
        if(commandForwarder) {
            commandForwarder->deinit();
            delete(commandForwarder);
        }
    }catch(...){
        
    }
    try{
        if(customForwarder) {
            customForwarder->deinit();
            delete(customForwarder);
        }
    }catch(...){
        
    }
    
    io_service.stop();
        // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < serviceThread.size(); ++i)
        serviceThread[i]->join();
}
    //! abstract queue action method implementation
bool AsioImplEventClient::submitEvent(EventDescriptor *event)  throw(CException) {
    bool result = true;
    switch (event->getEventType()) {
        case event::EventTypeAlert:
            result = alertForwarder->submitEventAsync(event);
            break;
        case event::EventTypeInstrument:
            result = instrumentForwarder->submitEventAsync(event);
            break;
        case event::EventTypeCommand:
            result = commandForwarder->submitEventAsync(event);
            break;
        case event::EventTypeCustom:
            result = customForwarder->submitEventAsync(event);
            break;
    }
    return result;
}