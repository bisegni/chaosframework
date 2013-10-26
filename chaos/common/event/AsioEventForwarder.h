/*
 *	AsioEventForwarder.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 26/08/12.
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

#ifndef __CHAOSFramework__AsioEventForwarder__
#define __CHAOSFramework__AsioEventForwarder__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace event{
        using namespace boost;
        class AsioImplEventClient;
        
        class AsioEventForwarder : public CObjectProcessingPriorityQueue<EventDescriptor> {
            friend class AsioImplEventClient;
            std::string hanlderID;
            
        protected:
            /*!
             init the asio event forwarder
             */
            void init() throw(CException);
            
            /*!
             deinit the asio event forwarder
             */
            void deinit() throw(CException);

            
        public:
                //! default constructor
            AsioEventForwarder(const boost::asio::ip::address& multicast_address,
                               unsigned short mPort,
                               boost::asio::io_service& io_service);
            
            virtual ~AsioEventForwarder();
            
                //! submit the event
            bool submitEventAsync(EventDescriptor *event);
            
                //! abstract queue action method implementation
            void processBufferElement(EventDescriptor *priorityElement, ElementManagingPolicy& policy) throw(CException);
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

#endif /* defined(__CHAOSFramework__AsioEventForwarder__) */
