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
#ifndef __CHAOSFramework__DirectIODeviceServerChannel__
#define __CHAOSFramework__DirectIODeviceServerChannel__

#include <chaos/common/chaos_types.h>
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
                public:
                    //! Device handler definition
                    typedef class DirectIODeviceServerChannelHandler {
                    public:
                        //! Receive the CDataWrapper forwarded by the channel
                        /*!
                         Receive the CdataWrapper forwarded by the channel, the deallocation is demanded to the handler
                         \param header the header of the channel api
                         \param channel_data the data sent by the device
                         \synchronous_answer possible async answer (not used for now)
                         */
                        virtual int consumePutEvent(const std::string& key,
                                                    const uint8_t hst_tag,
                                                    const ChaosStringSetConstSPtr meta_tag_set,
                                                    data::BufferSPtr channel_data)
                        {return -1;};
                        
                        //! Receive the CDataWrapper forwarded by the channel that contains the helath data
                        /*!
                         Receive the CDataWrapper forwarded by the channel, the deallocation is demanded to the handler
                         \param header the header of the channel api
                         \param channel_data contains the health data
                         */
                        virtual int consumeHealthDataEvent(const std::string& key,
                                                           const uint8_t hst_tag,
                                                           const ChaosStringSetConstSPtr meta_tag_set,
                                                           data::BufferSPtr channel_data)
                        {return -1;};
                        
                        //! Receive the key of the live data channel to read
                        /*!
                         Receive the key to fetch from the live cache and fill the synchronous_answer to return
                         in synchronous way the ansert to the client
                         \param key_data the data of the key
                         \param key_len the size of the key data
                         \param result_header the ehader that specify information of the result
                         \param result_value the values of the result
                         */
                        virtual int consumeGetEvent(chaos::common::data::BufferSPtr key_data,
                                                    uint32_t key_len,
                                                    opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult& result_header,
                                                    chaos::common::data::BufferSPtr& result_value)
                        {return -1;};
                        
                        //! Receive the et of keys of the live data channel to read
                        /*!
                         Receive the keys set to fetch from the live cache and fill the synchronous_answer to return
                         in synchronous way the result of the answre
                         \param header header containing the information where send the answer
                         \param keys the set of key to retrieve
                         \param result_header
                         \param result_header is the merory that contains the bson document answer for each key in sequence,
                         \param result_value is the size of memory allocated into result_value pointer
                         \param result_value
                         if a key is not found in live an empy bson document is add.
                         */
                        virtual int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode& header,
                                                    const ChaosStringVector& keys,
                                                    opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult& result_header,
                                                    chaos::common::data::BufferSPtr& result_value,
                                                    uint32_t& result_value_len)
                        {return -1;};
                        
                        //! Execute a paged query into a time intervall
                        /*!
                         Execute a paged query in sinchronous way
                         \param query_header of the request containing the naswer information
                         \param search_key the key that we need to query
                         \param search_start_ts the start of the time that delimit the lower time stamp of result
                         \param search_end_ts the end of the time stamp that delimit the upper time stamp of result
                         \param
                         */
                        virtual int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                                          const std::string& search_key,
                                                          const ChaosStringSet& meta_tags,
                                                          const uint64_t search_start_ts,
                                                          const uint64_t search_end_ts,
                                                          opcode_headers::SearchSequence& last_element_found_seq,
                                                          opcode_headers::QueryResultPage& result_page)
                        {return -1;};
                        
                        //! Execute a paged query into a time intervall returning only indexes
                        /*!
                         Execute a paged query in sinchronous way
                         \param query_header of the request containing the naswer information
                         \param search_key the key that we need to query
                         \param search_start_ts the start of the time that delimit the lower time stamp of result
                         \param search_end_ts the end of the time stamp that delimit the upper time stamp of result
                         \param
                         */
                        virtual int consumeDataIndexCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                                               const std::string& search_key,
                                                               const ChaosStringSet& meta_tags,
                                                               const uint64_t search_start_ts,
                                                               const uint64_t search_end_ts,
                                                               opcode_headers::SearchSequence& last_element_found_seq,
                                                               opcode_headers::QueryResultPage& result_page)
                        {return -1;};
                        
                        
                        //! Execute a paged query into a time intervall returning only indexes
                        /*!
                         Return data associated to the index, in fo ndata is found for a specific
                         index, an empty object will be created
                         \param indexes vector contains cdata wrapper abstraction for data index
                         \param data array taht will be filled with found data for each index
                         */
                        virtual int getDataByIndex(const chaos::common::data::VectorCDWShrdPtr& indexes,
                                                   chaos::common::data::VectorCDWShrdPtr& data)
                        {return -1;};
                        
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
                    
                    int consumeDataPack(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIODeviceServerChannel__) */
