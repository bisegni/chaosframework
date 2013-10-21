/*
 *	DirectIODataForwarder.h
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

#ifndef CHAOSFramework_DirectIODataForwarder_h
#define CHAOSFramework_DirectIODataForwarder_h

#include <stdint.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			
			//! Public interface fo the direct io client
			/*!
				This class represent the public interface that need to used to abstract the direct io client implementation.
			 */
			class DirectIODataForwarder {
			public:
				//! send the data to the server layer
				/*!
				 Forward the data throught the client sub layer for to be sent to the other side
				 */
				int sendData(void *data_buffer, uint32_t data_size);
			};
			
		}
	}
}

#endif
