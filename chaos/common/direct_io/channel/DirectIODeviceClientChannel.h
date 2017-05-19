/*
 *	DirectIODeviceClientChannel.h
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
#ifndef __CHAOSFramework__DirectIODeviceClientChannel__
#define __CHAOSFramework__DirectIODeviceClientChannel__

#include <string>
#include <stdint.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/DataBuffer.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace channel {
                
                typedef struct AnswerServerInfo {
                    uint16_t p_server_port;
                    uint16_t s_server_port;
                    uint16_t endpoint;
                    uint64_t ip;
                    uint32_t hash;
                } AnswerServerInfo;
                
                CHAOS_DEFINE_SET_FOR_TYPE(chaos::common::utility::DataBuffer<>, SetDataBuffer);
                
                //! Class for the managment of pushing data for the device dataset
                /*!
                 This class manage the forwarding of data that represent the device dataset channels (i/O)
                 */
                DECLARE_CLASS_FACTORY(DirectIODeviceClientChannel, DirectIOVirtualClientChannel) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceClientChannel)
                    
                    class DirectIODeviceClientChannelDeallocator:
                    public DirectIODeallocationHandler {
                    protected:
                        void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr);
                    };
                    //static deallocator forthis channel
                    static DirectIODeviceClientChannelDeallocator STATIC_DirectIODeviceClientChannelDeallocator;
                    
                    AnswerServerInfo answer_server_info;
                protected:
                    DirectIODeviceClientChannel(std::string alias);
                    
                public:
                    ~DirectIODeviceClientChannel();
                    
                    //! set the information on witch port forward the answer(the ip is the ip of the machine)
                    void setAnswerServerInfo(uint16_t p_server_port, uint16_t s_server_port, uint16_t answer_enpoint);
                    
                    //! Send device serialization with priority
                    int64_t storeAndCacheDataOutputChannel(const std::string& key,
                                                           void *buffer,
                                                           uint32_t buffer_len,
                                                           DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                           bool wait_result = true);
                    
                    //! Send device serialization with priority
                    int64_t storeAndCacheHealthData(const std::string& key,
                                                    void *buffer,
                                                    uint32_t buffer_len,
                                                    DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                    bool wait_result = true);
                    
                    //! Send a request for the last output data
                    int64_t requestLastOutputData(const std::string& key,
                                                  void **result, uint32_t &size);
                    
                    //! Send a request for the last output data for a set of key
                    int64_t requestLastOutputData(const ChaosStringVector& keys,
                                                  chaos::common::data::VectorCDWShrdPtr& results);
                    
                    //! Perform a temporal query on a key
                    /*!
                     Perform a query on a data cloud key(aka device id)
                     \param key to search
                     \param start_ts start of timestamp to search
                     \param end_ts end of the timestamp where limit the search
                     \param query_id the newly associated query id is returned.
                     \param result_handler has the found element page
                     \return error
                     */
                    int64_t queryDataCloud(const std::string& key,
                                           uint64_t start_ts,
                                           uint64_t end_ts,
                                           uint32_t page_dimension,
                                           uint64_t last_sequence_id,
                                           opcode_headers::DirectIODeviceChannelOpcodeQueryDataCloudResultPtr *result_handler);
                    
                    //! Perform a temporal data delete operation on a key
                    /*!
                     \param key to search
                     \param start_ts start of timestamp to search
                     \param end_ts end of the timestamp where limit the search
                     \return error
                     */
                    int64_t deleteDataCloud(const std::string& key,
                                            uint64_t start_ts,
                                            uint64_t end_ts);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIODeviceClientChannel__) */
