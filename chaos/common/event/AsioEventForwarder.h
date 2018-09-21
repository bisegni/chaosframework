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

#ifndef __CHAOSFramework__AsioEventForwarder__
#define __CHAOSFramework__AsioEventForwarder__
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace common {
        namespace event{
            using namespace boost;
            class AsioImplEventClient;
            
            class AsioEventForwarder :
            public chaos::common::pqueue::CObjectProcessingPriorityQueue<EventDescriptor> {
                friend class AsioImplEventClient;
                std::string hanlderID;
                
            protected:
                /*!
                 init the asio event forwarder
                 */
                void init();
                
                /*!
                 deinit the asio event forwarder
                 */
                void deinit();
                
                
            public:
                //! default constructor
                AsioEventForwarder(const boost::asio::ip::address& multicast_address,
                                   unsigned short mPort,
                                   boost::asio::io_service& io_service);
                
                virtual ~AsioEventForwarder();
                
                //! submit the event
                bool submitEventAsync(EventDescriptorSPtr event);
                
                //! abstract queue action method implementation
                void processBufferElement(EventDescriptorSPtr priorityElement);
            private:
                bool sent;
                //! mutext used for unlock and wait esclusive access
                boost::mutex wait_answer_mutex;
                //! condition variable for wait the answer
                boost::condition_variable wait_answer_condition;
                boost::asio::ip::udp::socket _socket;
                boost::asio::ip::udp::endpoint _endpoint;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__AsioEventForwarder__) */
