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
						//! Manage the creation of a snapshot
						/*!
						 The creation for a new snapshot has been requested, all information
						 on the live cache will be stored into database layer creating a
						 reference to this snapshot.
						 \param header header of the new snapshot api
						 \param snapped_producer_key the list of the producer, identfied the
						 unique key, to include into the snaphsoot
						 \param api_result the result of the api
						 \return error on the forwading of the event
						 */
						virtual int consumeNewSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
															void *concatenated_unique_id_memory,
															uint32_t concatenated_unique_id_memory_size,
															DirectIOSystemAPISnapshotResult *api_result)
						{DELETE_HEADER_DATA(header, concatenated_unique_id_memory) return 0;};
						
						//! Manage the delete operation on an existing snapshot
						/*!
						 Perform the delete operation on the snpashot and all dataset associated to it.
						\param header of the snapshot to delete
						\param api_result the result of the api
						\return error on the forwading of the event
						 */
						virtual int consumeDeleteSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
															   DirectIOSystemAPISnapshotResult *api_result)
						{DELETE_HEADER(header) return 0;};
						
						//! Return the dataset for a producerkey ona specific snapshot
						/*!
						\param header of the snapshot where to fetch the dataasets
						\param producer_id is the identification of the producre of the returning datasets
						\return error on the forwading of the event
						 */
						virtual int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
																   const std::string& producer_id,
																   void **channel_found_data,
																   uint32_t& channel_found_data_length,
																   DirectIOSystemAPISnapshotResult *api_result)
						{DELETE_HEADER(header) return 0;};
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
