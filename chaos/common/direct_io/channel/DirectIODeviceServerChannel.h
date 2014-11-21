/*
 *	DirectIODeviceServerChannel.h
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
#ifndef __CHAOSFramework__DirectIODeviceServerChannel__
#define __CHAOSFramework__DirectIODeviceServerChannel__


#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				//! Sercver channel for the managment of data request
				/*!
				 This channel answer to the client api for data managment, push, get last data and various query
				 */
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIODeviceServerChannel, DirectIOVirtualServerChannel), public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceServerChannel)
				public:
					//! Device handler definition
					typedef class DirectIODeviceServerChannelHandler {
                    public:
						//! Receive the CDataWrapper forwarded by the channel
						/*!
						 Receive the CdataWrapper forwarded by the channel, the deallocation is demanded to the handler
                         \param channel_opcode the opcode of the comand received by the channel
                         \param channel_header_ptr the abstract pointr for the channel header. Implementations need to cast it according to opcode value
                         \param channel_data the data (if speified by channel)
						 \synchronous_answer possible async answer (not used for now)
						 */
						virtual int consumePutEvent(opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
													 void *channel_data,
													 uint32_t channel_data_len,
													 DirectIOSynchronousAnswerPtr synchronous_answer)
						{DELETE_HEADER_DATA(header, channel_data) return 0;};
						
						//! Receive the key of the live data channel to read
						/*!
							Receive the key to fetch from the live cache and fill the synchronous_answer to return
							in synchronous way the ansert to the client
						 \param header header containing the information where send the answer
						 \param key_data the data of the key
						 \param key_len the size of the key data
						 \param synchronous_answer the answer structure to return the found value associated to the key
								on the live system
						 */
						virtual int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
													 void *key_data,
													 uint32_t key_len,
													 DirectIOSynchronousAnswerPtr synchronous_answer)
						{DELETE_HEADER_DATA(header, key_data) return 0;};
						
						//! Receive the query information for search on data cloud
						/*!
							The CData wrapper received contains all the infromation needed to perform query 
							on chaos data cloud. The answer need to be sent in asynchornous way.
						 \param header of the request containing the naswer information
						 \param search_key the key that we need to query
						 \param search_start_ts the start of the time that delimit the lower time stamp of result
						 \param search_end_ts the end of the time stamp that delimit the upper time stamp of result
						 \param synchronous_answer the synchronous answer (not used at the moment)
						 */
						virtual int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header,
														  const std::string& search_key,
														  uint64_t search_start_ts,
														  uint64_t search_end_ts,
														  DirectIOSynchronousAnswerPtr synchronous_answer)
						{DELETE_HEADER(header) return 0;};
						
						//! Receive the start result answering sequence for a query from the quered node
						/*!
						 This message start the answering sequence composed by many consumeDataCloudQueryAnswer method call. It bring infromation
						 about overall query metadata like the total number of element found. In future this will be used to inform clinet
						 to the query splitting across server.
						 */
						virtual int consumeDataCloudQueryStartResult(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult *header)
						{DELETE_HEADER(header) return 0;}
						
						//! Receive the result to a submitted query
						/*!
						 the query id is contained into header and the data point to the single data pack
						 \param header the hader fo the answer
						 \param data_found the single datapack found
						 \param data_lenght the lenght of the data pack
						 \param synchronous_answer the synchronous answer (not used at the moment)
						 */
						virtual int consumeDataCloudQueryResult(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult *header,
																void *data_found,
																uint32_t data_lenght,
																DirectIOSynchronousAnswerPtr synchronous_answer)
						{DELETE_HEADER_DATA(header, data_found) return 0;};
						
						//! Receive the end message for the results answering sequence from the answering node
						/*!
						 This is need to manage the error occurend on the server ansering node, to permit the request node
						 to not way any further result.
						 \param header the header of the end result phase
						 \param error_message_string_data the error messag eif there is one
						 \param error_message_string_data_length error message lenght
						 */
						virtual int consumeDataCloudQueryEndResult(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult *header,
																   void *error_message_string_data,
																   uint32_t error_message_string_data_length)
						{DELETE_HEADER_DATA(header, error_message_string_data) return 0;}

					} DirectIODeviceServerChannelHandler;

                    void setHandler(DirectIODeviceServerChannelHandler *_handler);
				protected:
					//! Handler for the event
					DirectIODeviceServerChannelHandler *handler;
					
					DirectIODeviceServerChannel(std::string alias);
					
					int consumeDataPack(DirectIODataPack *dataPack, DirectIOSynchronousAnswerPtr synchronous_answer);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIODeviceServerChannel__) */
