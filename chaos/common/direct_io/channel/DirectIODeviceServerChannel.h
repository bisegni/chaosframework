/*
 *	DirectIODeviceServerChannel.h
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
				DECLARE_CLASS_FACTORY(DirectIODeviceServerChannel, DirectIOVirtualServerChannel),
                public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceServerChannel)
                    
                    class DirectIODeviceServerChannelDeallocator:
                    public DirectIODeallocationHandler {
                    protected:
                        void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr);
                    };
                    //static deallocator forthis channel
                    static DirectIODeviceServerChannelDeallocator STATIC_DirectIODeviceServerChannelDeallocator;
                    
                    
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
													 uint32_t channel_data_len)
						{DELETE_HEADER_DATA(header, channel_data) return -1;};
						
						//! Receive the key of the live data channel to read
						/*!
							Receive the key to fetch from the live cache and fill the synchronous_answer to return
							in synchronous way the ansert to the client
						 \param header header containing the information where send the answer
						 \param key_data the data of the key
						 \param key_len the size of the key data
						 \param result_header
                         \param result_value
						 */
						virtual int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
                                                    void *key_data,
                                                    uint32_t key_len,
                                                    opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                                    void **result_value)
						{DELETE_HEADER_DATA(header, key_data) return -1;};
                        
						//! Execute a paged query into a time intervall
						/*!
							Execute a paged query in sinchronous way
						 \param header of the request containing the naswer information
						 \param search_key the key that we need to query
						 \param search_start_ts the start of the time that delimit the lower time stamp of result
						 \param search_end_ts the end of the time stamp that delimit the upper time stamp of result
						 \param start_ts_is_included true if the search_start_ts need to be considere included or not
                         \param
						 */
						virtual int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud *query_header,
                                                          const std::string& search_key,
														  uint64_t search_start_ts,
														  uint64_t search_end_ts,
                                                          bool start_ts_is_included,
                                                          opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult * result_header,
                                                          void **result_value)
						{DELETE_HEADER(query_header) return -1;};
                        
                        //! Delete the data for a key delimited into a time intervall
                        /*!                         
                         \param search_key the key for wich we need to delete data
                         \param start_ts the timestamp that limit >= the time after wich to delete data (0 means no limit)
                         \param end_ts the timestamp that limit =< the time before wich wich to delete data (0 means no limit)
                         */
                        virtual int consumeDataCloudDelete(const std::string& search_key,
                                                           uint64_t start_ts,
                                                           uint64_t end_ts)
                        {return -1;};
					} DirectIODeviceServerChannelHandler;

                    void setHandler(DirectIODeviceServerChannelHandler *_handler);
				protected:
					//! Handler for the event
					DirectIODeviceServerChannelHandler *handler;
					
					DirectIODeviceServerChannel(std::string alias);
					
					int consumeDataPack(DirectIODataPack *dataPack,
                                        DirectIODataPack *synchronous_answer,
                                        DirectIODeallocationHandler **answer_header_deallocation_handler,
                                        DirectIODeallocationHandler **answer_data_deallocation_handler);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIODeviceServerChannel__) */
