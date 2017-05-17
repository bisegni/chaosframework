/*
 *	AsioImplEventClient.h
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

#ifndef __CHAOSFramework__AsioImplEventClient__
#define __CHAOSFramework__AsioImplEventClient__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/event/EventClient.h>
#include <chaos/common/event/AsioEventHandler.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace event{
            using namespace std;
            using namespace boost;
            
            class AsioEventForwarder;
            
            DECLARE_CLASS_FACTORY(AsioImplEventClient, EventClient),
            public chaos::common::pqueue::CObjectProcessingPriorityQueue<EventDescriptor>{
            protected:
                /*
                 init the event adapter
                 */
                void init(void*) throw(CException);
                
                /*
                 start the event adapter
                 */
                void start() throw(CException);
                
                //-----------------------
                void stop() throw(CException);
                
                /*
                 deinit the event adapter
                 */
                void deinit() throw(CException);
                
                //! abstract queue action method implementation
                void processBufferElement(EventDescriptor *priorityElement,
                                          ElementManagingPolicy& policy) throw(CException);
                
            public:
                AsioImplEventClient(string alias);
                ~AsioImplEventClient();
                bool submitEvent(EventDescriptor *event)  throw(CException);
            private:
                boost::asio::ip::udp::socket *socket_alert;
                boost::asio::ip::udp::socket *socket_instrument;
                boost::asio::ip::udp::socket *socket_command;
                boost::asio::ip::udp::socket *socket_custom;
                
                boost::asio::io_service io_service;
                boost::thread_group	service_thread_group;
                //vector< ChaosSharedPtr<boost::thread> > serviceThread;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__AsioImplEventClient__) */
