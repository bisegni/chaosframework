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
#ifndef CHAOSFramework_DirectIOHandler_h
#define CHAOSFramework_DirectIOHandler_h

#include <stdint.h>
#include <chaos/common/direct_io/DirectIOTypes.h>
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
                 \param synchronous_answer the datapacke for the sinchronous answer.
				 */
                virtual int priorityDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                 chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) = 0;
                
                //! Event for a new data received
				/*!
				 This permit to inform the imlementator tha a new data has received by the server.
				 \param data_buffer is an handle to avoid poiter that will contains the pointer to the received data.
				 \param data_size the size of the received data
				 \param detach inform the server that hte memory buffed doesn't need to be deallocated.
                 \param synchronous_answer the datapacke for the sinchronous answer.
				 */
                virtual int serviceDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) = 0;
			};
			
            typedef int (chaos::common::direct_io::DirectIOHandler::*DirectIOHandlerPtr)(chaos::common::direct_io::DirectIODataPackUPtr& data_pack,
                                                                                         chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
			
			#define DirectIOHandlerPtrCaller(instance,variable) ((*instance).*variable)
		}
	}
}
#endif
