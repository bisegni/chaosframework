/*
 *	DirectIOServerHandler.h
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
#ifndef CHAOSFramework_DirectIOServerHandler_h
#define CHAOSFramework_DirectIOServerHandler_h


namespace chaos {
	namespace common {
		namespace direct_io {
			//! Public interface fo the direct io server
			/*!
			 This class represent the public interface that need to used to abstract the direct io server implementation.
			 Another class to became the server implementation need to be extends this interface to permit the data exchange data betwin the client.
			 */
			class DirectIODataHandler {
			public:
				
				//! Event for a new data received
				/*!
				 This permit to inform the imlementator tha a new data has received by the server.
				 \param data_buffer is an handle to avoid poiter that will contains the pointer to the received data.
				 \param data_size the size of the received data
				 \param detach inform the server that hte memory buffed doesn't need to be deallocated.
				 */
				void dataReceived(void **data_buffer, uint32_t data_size, bool& detach);
			};
		}
	}
}
#endif
