/*
 *	DirectIOHandler.h
 *	!CHAOS
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
#ifndef CHAOSFramework_DirectIOHandler_h
#define CHAOSFramework_DirectIOHandler_h

#include <stdint.h>
#include <chaos/common/direct_io/DirectIODataPack.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			
			//! Public interface fo the direct io server
			/*!
			 This class represent the public interface that need to used to abstract the direct io server implementation.
			 Another class to became the server implementation need to be extends this interface to permit the data exchange data betwin the client.
			 */
			class DirectIOHandler {
			public:
				
				//! Event for a new data received
				/*!
				 This permit to inform the imlementator tha a new data has received by the server.
				 \param data_buffer is an handle to avoid poiter that will contains the pointer to the received data.
				 \param data_size the size of the received data
				 \param detach inform the server that hte memory buffed doesn't need to be deallocated.
				 */
				virtual int priorityDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer) = 0;
                
                //! Event for a new data received
				/*!
				 This permit to inform the imlementator tha a new data has received by the server.
				 \param data_buffer is an handle to avoid poiter that will contains the pointer to the received data.
				 \param data_size the size of the received data
				 \param detach inform the server that hte memory buffed doesn't need to be deallocated.
				 */
				virtual int serviceDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer) = 0;
			};
			
			typedef int (chaos::common::direct_io::DirectIOHandler::*DirectIOHandlerPtr)(chaos::common::direct_io::DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer);
			
			#define DirectIOHandlerPtrCaller(instance,variable) ((*instance).*variable)
		}
	}
}
#endif
