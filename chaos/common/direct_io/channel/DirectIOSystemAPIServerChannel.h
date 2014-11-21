/*
 *	DirectIOPerformanceServerChannel.h
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

#ifndef __CHAOSFramework__DirectIOSystemAPIServerChannel__
#define __CHAOSFramework__DirectIOSystemAPIServerChannel__

#include <string>
#include <vector>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIODispatcher;
			namespace channel {
				using namespace chaos::common::direct_io::channel::opcode_headers;
				//! serve rimplementation for the System API direct io channel
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIOSystemAPIServerChannel, DirectIOVirtualServerChannel),
				public chaos::common::direct_io::DirectIOEndpointHandler {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOSystemAPIServerChannel)
				public:
					//! System API DirectIO server handler
					typedef class DirectIOSystemAPIServerChannelHandler {
					public:
						//! Receive the SystemAPIChannelOpcodeNewSnapshootDatasetNew opcode
						/*!
						 The creation for a new snapshot has been requested, all information
						 on the live cache will be stored into database layer creating a 
						 reference to this snapshot.
						 \param header header of the new snapshot api
						 \param snapped_producer_key the list of the producer, identfied the
						unique key, to include into the snaphsoot
						 */
						virtual int consumeNewSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNewSnapshootHeader *header,
															void *concatenated_unique_id_memory,
															uint32_t concatenated_unique_id_memory_size,
															DirectIOSystemAPINewSnapshootResult *api_result)
						{DELETE_HEADER_DATA(header, concatenated_unique_id_memory) return 0;};
						
					} DirectIOSystemAPIServerChannelHandler;
					
					void setHandler(DirectIOSystemAPIServerChannelHandler *_handler);
				protected:
					//! Handler for the event
					DirectIOSystemAPIServerChannelHandler *handler;
					
					//!default constructor
					DirectIOSystemAPIServerChannel(std::string alias);
					
					//! endpoint entry method
					int consumeDataPack(DirectIODataPack *dataPack,
										DirectIOSynchronousAnswerPtr synchronous_answer);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOSystemAPIServerChannel__) */
