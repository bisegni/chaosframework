/*
 *	EventServer.h
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

#ifndef CHAOSFramework_EventServer_h
#define CHAOSFramework_EventServer_h
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/StartableService.h>


namespace chaos {
	namespace common {
		namespace network {
			class NetworkBroker;
		}
	}
	
    namespace event{
        
        class EventServer:
		public common::utility::NamedService,
		public chaos::common::utility::StartableService {
            friend class chaos::common::network::NetworkBroker;
                //! handler that can manage the event reception
            EventHandler *rootEventHandler;
        protected:
                //Default Server Constructor
            EventServer(const string& alias);
            
            /*
             init the event adapter
             */
            virtual void init(void*) throw(CException) = 0;
            
            /*
             start the event adapter
             */
            virtual void start() throw(CException) = 0;
            
            virtual void stop() throw(CException) = 0;
            
            /*
             start the event adapter
             */
            virtual void listeForEventType(event::EventType type,  bool listen) throw(CException) = 0;

            
            /*
             deinit the event adapter
             */
            virtual void deinit() throw(CException) = 0;
            
            void dispatchEventToHandler(const unsigned char * const serializedEvent, uint16_t length) throw (CException);

        public:
            
            void setEventHanlder(EventHandler *eventHanlder);
        };
    }
}

#endif
