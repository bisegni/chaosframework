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

#include <chaos/common/event/AsioEventHandler.h>
#include <chaos/common/event/EventClient.h>
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
        
        class AsioEventForwarder;
        
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(AsioImplEventClient, EventClient) {
        protected:
            /*
             init the event adapter
             */
            void init(CDataWrapper*) throw(CException);
            
            /*
             start the event adapter
             */
            void start() throw(CException);
            
            /*
             deinit the event adapter
             */
            void deinit() throw(CException);

        public:
            AsioImplEventClient(string *alias);
            ~AsioImplEventClient();
            bool submitEvent(EventDescriptor *event)  throw(CException);
        private:
                //! Allert forwarder
            AsioEventForwarder *alertForwarder;
           
            //! Instrument forwarder
            AsioEventForwarder *instrumentForwarder;
            
            //! Command forwarder
            AsioEventForwarder *commandForwarder;
            
            //! Custom forwarder
            AsioEventForwarder *customForwarder;
            
            boost::asio::io_service io_service;
            vector< shared_ptr<boost::thread> > serviceThread;
        };
    }
}

#endif /* defined(__CHAOSFramework__AsioImplEventClient__) */
