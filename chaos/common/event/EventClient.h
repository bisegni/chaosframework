/*
 *	EventClient.h
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

#ifndef __CHAOSFramework__EventClient__
#define __CHAOSFramework__EventClient__
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/event/EventForwarder.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/utility/StartableService.h>
namespace chaos {
	namespace common {
		namespace network {
			class NetworkBroker;			
		}
	}
	
    namespace event {
        
            //!Event client base class
        /*!
         
         */
        class EventClient:
		public common::utility::NamedService,
		public EventForwarder,
		public chaos::common::utility::StartableService {
            friend class chaos::common::network::NetworkBroker;
            
        protected:
            uint8_t threadNumber;            
                //Default Server Constructor
            EventClient(std::string alias);
            
            /*
             init the event adapter
             */
            virtual void init(void*) throw(CException);
            
            /*
             start the event adapter
             */
            virtual void start() throw(CException);
            
            //-----------------------
            virtual void stop() throw(CException){};
            
            /*
             deinit the event adapter
             */
            virtual void deinit() throw(CException);
            
        public:
            void setThreadNumber(unsigned int newThreadNumber);
            unsigned int getThreadNumber();
        };

    }
}

#endif /* defined(__CHAOSFramework__EventClient__) */
