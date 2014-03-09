/*
 *	DirectIOClientConnection.h
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

#ifndef __CHAOSFramework__DirectIOClientStartPoint__
#define __CHAOSFramework__DirectIOClientStartPoint__

#include <map>
#include <string>

#include <boost/thread.hpp>

#include <chaos/common/utility/TemplatedKeyObjectContainer.h>

#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos {
	namespace common {
		namespace direct_io {
            //forward delcarations
            class DirectIOClient;
            class DirectIOClientConnection;
            
            //! Connection event enumeration
            typedef enum DirectIOClientConnectionEventType {
                DirectIOClientConnectionEventConnected,
                DirectIOClientConnectionEventDisconnected
            } DirectIOClientConnectionEventType;
            
            
            //! Direct io client connection event handler
            class DirectIOClientConnectionEventHandler {
            protected:
                //! han
                virtual void handleEvent(DirectIOClientConnectionEventType event, void *event_data, uint32_t even_data_len) = 0;
            };
            
            //! Represent the start point of a messaget towards an endpoint
            /*!
                This class is the start point for a comunication winth a server endpoint,
                In definitive it can be viwed as a socket connected to a server::port that
                need to send the data towards an endpoint
             */
            class DirectIOClientConnection  : protected utility::TemplatedKeyObjectContainer< unsigned int, channel::DirectIOVirtualClientChannel> {
                friend class DirectIOClient;
                DirectIOClientConnection(std::string server_description, bool priority);
                
            protected:
                DirectIOClientConnectionEventHandler *event_handler;
            public:
                virtual ~DirectIOClientConnection();
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				channel::DirectIOVirtualClientChannel *getNewChannelInstance(std::string channel_name);
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				void releaseChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOClientStartPoint__) */
