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
#ifndef CHAOSFramework_DirectIODataForwarder_h
#define CHAOSFramework_DirectIODataForwarder_h

#include <stdint.h>
#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/DirectIODataPack.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				class DirectIOVirtualClientChannel;
			}
			
			//! Public interface fo the direct io client
			/*!
				This class represent the public interface that need to used to abstract the direct io client implementation.
			 */
			class DirectIOForwarder {
			public:
				
				//! comodity static method for deallocation managment
				static void freeSentData(void *data, void *hint) {
					DisposeSentMemoryInfo *free_info = static_cast<DisposeSentMemoryInfo*>(hint);
					free_info->data_deallocator->freeSentData(data, free_info);
					delete(free_info);
				}
				
				//! send the data to the server layer on priority channel
				/*!
				 Forward the data throught the client sub layer for to be sent to the other side
                 using hte priority channel. This channel is to be used for the fast data push.
				 */
				virtual int64_t sendPriorityData(DirectIODataPack *data_pack,
												 DirectIODeallocationHandler *header_deallocation_handler,
												 DirectIODeallocationHandler *data_deallocation_handler,
												 DirectIODataPack **asynchronous_answer = NULL) = 0;

                
                //! send the data to the server layer on the service channel
				/*!
				 Forward the data throught the client sub layer for to be sent to the other side
                 using the service channel. This channel is to be used for the data that doesn't need
                 to be fast and accurate.
				 */
				virtual int64_t sendServiceData(DirectIODataPack *data_pack,
												DirectIODeallocationHandler *header_deallocation_handler,
												DirectIODeallocationHandler *data_deallocation_handler,
												DirectIODataPack **asynchronous_answer = NULL) = 0;
			};
			
			typedef int64_t (chaos::common::direct_io::DirectIOForwarder::*DirectIOForwarderHandler)(chaos::common::direct_io::channel::DirectIOVirtualClientChannel *channel,
																									 chaos::common::direct_io::DirectIODataPack *data_pack,
																									 DirectIODataPack **asynchronous_answer);
			
			#define DirectIOForwarderHandlerCaller(instance,variable) ((*instance).*variable)
		}
	}
}

#endif
