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

#ifndef __CHAOSFramework__AsioImplEventServer__
#define __CHAOSFramework__AsioImplEventServer__

#include <chaos/common/event/AsioEventHandler.h>
#include <chaos/common/event/EventServer.h>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace event{
        using namespace std;
        using namespace boost;
        
        const short multicast_port = 30001;
        class AsioEventHandler;
        
        //! Asio impleentation for the !CHOAS event server
        /*!
         
         */
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(AsioImplEventServer, EventServer) {
            friend class AsioEventHandler;
            
            //! alert socket
            AsioEventHandler *alertHandler;
            
            //! istrument socket
            AsioEventHandler *instrumentHandler;
            
            //! command socket
            AsioEventHandler *commandHandler;
            
            //! custom socket
            AsioEventHandler *customHandler;
            
        protected:
            /*
             init the event adapter
             */
            void init(void*) throw(CException);
            
            /*
             start the event adapter
             */
            void start() throw(CException);
            
            /*
             stop the event adapter
             */
            void stop() throw(CException);
            
            /*
             register or deregister for a kynd of event
             */
            virtual void listeForEventType(event::EventType type,  bool listen) throw(CException);
            
            /*
             deinit the event adapter
             */
            void deinit() throw(CException);
            
            void sendEventDataToRootHandler(unsigned char * buff, uint16_t length);
        public:
            AsioImplEventServer(string *alias);
            
        private:
            boost::asio::io_service io_service;
			boost::thread_group serviceThreads;
            //vector< shared_ptr<boost::thread> > serviceThread;
            uint8_t threadNumber;
            
        };
    }
}
#endif /* defined(__CHAOSFramework__AsioImplEventServer__) */
