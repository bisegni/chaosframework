/*
 *	AsioImplEventServer.h
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
#include <chaos/common/chaos_constants.h>
#include <chaos/common/global.h>
#include <chaos/common/event/AsioImplEventServer.h>

using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::event;

#define DEFAULT_ALERT_EVENT_PORT        5000
#define DEFAULT_INSTRUMENT_EVENT_PORT   5001
#define DEFAULT_COMMAND_EVENT_PORT      5002
#define DEFAULT_CUSTOM_EVENT_PORT       5003

#define DEFAULT_BASE_IP                 "0.0.0.0"

AsioImplEventServer::AsioImplEventServer(string alias):EventServer(alias) {
    threadNumber = 0;
    alertHandler = NULL;
    instrumentHandler = NULL;
    commandHandler = NULL;
    customHandler = NULL;

}

/*
 init the event adapter
 */
void AsioImplEventServer::init(void*) throw(CException) {
    threadNumber = 1;
}

/*
 start the event adapter
 */
void AsioImplEventServer::start() throw(CException) {
    
        //listen for event
    listeForEventType(event::EventTypeAlert, true);
    listeForEventType(event::EventTypeInstrument, true);
    
    for (int idx = 0; idx < threadNumber; idx++) {
            //create the handler
        //shared_ptr<thread> thread(new boost::thread(bind(&asio::io_service::run, &io_service)));
        serviceThreads.create_thread(bind(&asio::io_service::run, &io_service));
    }
}

/*
 start the event adapter
 */
void AsioImplEventServer::stop() throw(CException) {
    
}

/*
 start the event adapter
 */
void AsioImplEventServer::listeForEventType(event::EventType type,  bool listen) throw(CException) {
    switch (type) {
        case event::EventTypeAlert:
            if(listen){
                if(alertHandler) return;
                alertHandler = new AsioEventHandler(asio::ip::address::from_string(DEFAULT_BASE_IP),
                                                    asio::ip::address::from_string(EventConfiguration::CONF_EVENT_ALERT_MADDRESS),
                                                    io_service, EventConfiguration::CONF_EVENT_PORT);
                alertHandler->asioServer = this;
            }else{
                if(alertHandler) delete(alertHandler);
            }
            break;
        case event::EventTypeInstrument:
            if(listen){
                if(instrumentHandler) return;
                
                instrumentHandler = new AsioEventHandler(asio::ip::address::from_string(DEFAULT_BASE_IP),
                                                         asio::ip::address::from_string(EventConfiguration::CONF_EVENT_INSTRUMENT_MADDRESS),
                                                         io_service, EventConfiguration::CONF_EVENT_PORT);
                instrumentHandler->asioServer = this;
            }else{
                if(instrumentHandler) delete(instrumentHandler);
            }
            break;
        case event::EventTypeCommand:
            if(listen){
                if(commandHandler) return;
                
                commandHandler = new AsioEventHandler(asio::ip::address::from_string(DEFAULT_BASE_IP),
                                                      asio::ip::address::from_string(EventConfiguration::CONF_EVENT_COMMAND_MADDRESS),
                                                      io_service, EventConfiguration::CONF_EVENT_PORT);
                commandHandler->asioServer = this;
            }else{
                if(commandHandler) delete(commandHandler);
            }
            break;
        case event::EventTypeCustom:
            if(listen){
                if(customHandler) return;
                
                customHandler = new AsioEventHandler(asio::ip::address::from_string(DEFAULT_BASE_IP),
                                                     asio::ip::address::from_string(EventConfiguration::CONF_EVENT_CUSTOM_MADDRESS),
                                                     io_service, EventConfiguration::CONF_EVENT_PORT);
                customHandler->asioServer = this;
            }else{
                if(customHandler) delete(customHandler);
            }
            break;
    }
}

/*
 deinit the event adapter
 */
void AsioImplEventServer::deinit() throw(CException) {
    
    //deregister all event listener
    try{listeForEventType(event::EventTypeAlert, false);}catch(...){}
    try{listeForEventType(event::EventTypeInstrument, false);}catch(...){}
    try{listeForEventType(event::EventTypeCommand, false);}catch(...){}
    try{listeForEventType(event::EventTypeCustom, false);}catch(...){}
    
    // delete al asio thread
    io_service.stop();
        // Wait for all threads in the pool to exit.
    //for (std::size_t i = 0; i < serviceThread.size(); ++i)
        //serviceThread[i]->join();
	serviceThreads.join_all();
}

    //-----------------------
void AsioImplEventServer::sendEventDataToRootHandler(unsigned char * buff, uint16_t length) {
    dispatchEventToHandler(buff, length);
}
