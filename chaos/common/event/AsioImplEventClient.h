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

#ifndef __CHAOSFramework__AsioImplEventClient__
#define __CHAOSFramework__AsioImplEventClient__
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

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
            public chaos::common::pqueue::CObjectProcessingPriorityQueue<EventDescriptor> {
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
                void processBufferElement(EventDescriptorSPtr priorityElement) throw(CException);
                
            public:
                AsioImplEventClient(string alias);
                ~AsioImplEventClient();
                bool submitEvent(EventDescriptorSPtr event)  throw(CException);
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
