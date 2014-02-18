/*
 *	DirectIOForwarder.h
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
#include <chaos/common/direct_io/DirectIODataPack.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			
			//! Public interface fo the direct io client
			/*!
				This class represent the public interface that need to used to abstract the direct io client implementation.
			 */
			class DirectIOForwarder {
			public:
				//! send the data to the server layer on priority channel
				/*!
				 Forward the data throught the client sub layer for to be sent to the other side
                 using hte priority channel. This channel is to be used for the fast data push.
				 */
				virtual int64_t sendPriorityData(DirectIODataPack *data_pack) = 0;

                
                //! send the data to the server layer on the service channel
				/*!
				 Forward the data throught the client sub layer for to be sent to the other side
                 using the service channel. This channel is to be used for the data that doesn't need
                 to be fast and accurate.
				 */
				virtual int64_t sendServiceData(DirectIODataPack *data_pack) = 0;
			};
			
			typedef int64_t (chaos::common::direct_io::DirectIOForwarder::*DirectIOForwarderHandler)(chaos::common::direct_io::DirectIODataPack *data_pack);
			
			#define DirectIOForwarderHandlerCaller(instance,variable) ((*instance).*variable)
		}
	}
}

#endif
