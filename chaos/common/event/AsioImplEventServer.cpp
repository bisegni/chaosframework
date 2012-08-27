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
#include <chaos/common/cconstants.h>
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

AsioImplEventServer::AsioImplEventServer(string *alias):EventServer(alias) {
    threadNumber = 0;
}

/*
 init the event adapter
 */
void AsioImplEventServer::init(CDataWrapper*) throw(CException) {
    threadNumber = 1;
}

/*
 start the event adapter
 */
void AsioImplEventServer::start() throw(CException) {

        //register forall event
        //create the services
    shared_ptr<AsioEventHandler> handler(new AsioEventHandler(asio::ip::address::from_string(DEFAULT_BASE_IP),
                                                              asio::ip::address::from_string(EventConfiguration::CONF_EVENT_ALERT_MADDRESS),
                                                              io_service, EventConfiguration::CONF_EVENT_PORT));
    handler->asioServer = this;
    
    handlerVec.push_back(handler);

    for (int idx = 0; idx < threadNumber; idx++) {
            //create the handler
        shared_ptr<thread> thread(new boost::thread(bind(&asio::io_service::run, &io_service)));
        serviceThread.push_back(thread);
    }
}

/*
 deinit the event adapter
 */
void AsioImplEventServer::deinit() throw(CException) {
    io_service.stop();
        // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < serviceThread.size(); ++i)
        serviceThread[i]->join();
    
}

    //-----------------------
void AsioImplEventServer::sendEventDataToRootHandler(unsigned char * buff, uint16_t length) {
     dispatchEventToHandler(buff, length);
}
