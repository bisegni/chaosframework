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
#ifndef __CHAOSFramework__DataBroker__
#define __CHAOSFramework__DataBroker__

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos {
	namespace common {
		
		namespace data {
			//! Namespace for grouping all class needed by the DataBroker services
			/*!
			 The Data Broker represent the principal node that manage the data sharing between node.
			 It is composed by a reader layer and publisher layers. Both layers can operate, using either
			 RPC or DirectI/O protocoll, in two mode poll and push.
			 \image html data_broker_scheme.png
			 */
			namespace broker {
				
				//! the remote choas node id that want conenct to thedata broker
#define REMOTE_NODE_ID "nodeID"
				//! the remote chaos node address
#define REMOTE_NODE_ADDR "nodeAddr"
				//! the requested mode to be used in the handshake initialization
#define REQUESTED_MODE "mode"
				
				/*!
				 Define the mode used by data broker
				 */
				typedef enum {
					/*!
					 Poll is the mode in with the client ask, when t wan't, the last value
					 */
					DATA_BROKER_POLL = 0,
					
					/*!
					 Push is the mode in with the client register it'self ofr receive the data updates
					 when it are present
					 */
					DATA_BROKER_PUSH
				} DataBrokerMode;
				
				
				//! Data Broker central managment class
				/*!
				 The data broker class is the central point where the push data service is managed.
				 Thsi class permit to create and ambiet(publisher) defined for a key(deviceID or domain)
				 where data is pushed and forwarded (push mode) to the remote listener or sent to the remote
				 node that request it(poll mode).
				 
				 This is also the class where is managed the handshake for registration of the remote listeners.
				 */
				class DataBroker:
				public utility::StartableService,
				public chaos::DeclareAction {
					
				public:
					//! Initialize the data broker
					void init(void*) ;
					//! start the data broker
					void start() ;
					//! stop the data broker
					void stop() ;
					//! deinit the data broker
					void deinit() ;
					//! action for start the handshake
                    chaos::common::data::CDWUniquePtr startHandshake(chaos::common::data::CDWUniquePtr handshake_data);
				};
			}
		}
    }
}

#endif /* defined(__CHAOSFramework__DataBroker__) */
