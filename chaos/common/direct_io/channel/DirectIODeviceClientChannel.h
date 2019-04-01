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
                
                CHAOS_DEFINE_SET_FOR_TYPE(chaos::common::utility::DataBuffer, SetDataBuffer);
                
                //! Class for the managment of pushing data for the device dataset
                /*!
                 This class manage the forwarding of data that represent the device dataset channels (i/O)
                 */
                DECLARE_CLASS_FACTORY(DirectIODeviceClientChannel, DirectIOVirtualClientChannel) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceClientChannel)
                protected:
                    DirectIODeviceClientChannel(std::string alias);
                public:
                    ~DirectIODeviceClientChannel();
                    
                    //! set the information on witch port forward the answer(the ip is the ip of the machine)
                    void setAnswerServerInfo(uint16_t p_server_port, uint16_t s_server_port, uint16_t answer_enpoint);
                    
                    //! Send device serialization with priority
                    int storeAndCacheDataOutputChannel(const std::string& key,
                                                       void *buffer,
                                                       uint32_t buffer_len,
                                                       DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                       const ChaosStringSet& tag_set = ChaosStringSet(),
                                                       bool wait_result = true);
                    
                    //! Send device serialization with priority
                    int storeAndCacheHealthData(const std::string& key,
                                                void *buffer,
                                                uint32_t buffer_len,
                                                DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                const ChaosStringSet& tag_set = ChaosStringSet(),
                                                bool wait_result = true);
                    
                    //! Send a request for the last output data
                    int requestLastOutputData(const std::string& key,
                                              char **result, uint32_t &size);
                    
                    //! Send a request for the last output data for a set of key
                    int requestLastOutputData(const ChaosStringVector& keys,
                                              chaos::common::data::VectorCDWShrdPtr& results);
                    
                    //! Perform a temporal query on a key
                    /*!
                     Perform a query on a data cloud key(aka device id)
                     \param key to search
                     \param start_ts start of timestamp to search
                     \param end_ts end of the timestamp where limit the search
                     \param last_sequence is an input-outpu field that permit to give sequence of the last found
                     element and will be filled with last element's sequencefo the current found page
                     \param found_element_page the page that contins the found data
                     \return error
                     */
                    int queryDataCloud(const std::string& key,
                                       const ChaosStringSet& meta_tags,
                                       const uint64_t start_ts,
                                       const uint64_t end_ts,
                                       const uint32_t page_dimension,
                                       opcode_headers::SearchSequence& last_sequence,
                                       opcode_headers::QueryResultPage& found_element_page,
                                       bool only_index = false);
                    
                    
                    //! Fetch the data associated to the indexs
                    /*!
                     If no data is found for and index, an emtpy object is created for that not found data
                     \param indexs vector of CDataWrapper that is found by @queryDataCloudIndexOnly
                     \param results the foudn data for the index
                     */
                    int getDataByIndex(const chaos::common::data::VectorCDWShrdPtr& indexs,
                                       chaos::common::data::VectorCDWShrdPtr& results);
                    
                    //! Fetch the data associated to the indexs
                    /*!
                     If no data is found for and index, an emtpy object is created for that not found data
                     \param indexs vector of CDataWrapper that is found by @queryDataCloudIndexOnly
                     \param results the foudn data for the index
                     */
                    int getDataByIndex(const chaos::common::data::CDWShrdPtr& index,
                                       chaos::common::data::CDWShrdPtr& result);
                    
                    //! Perform a temporal data delete operation on a key
                    /*!
                     \param key to search
                     \param start_ts start of timestamp to search
                     \param end_ts end of the timestamp where limit the search
                     \return error
                     */
                    int deleteDataCloud(const std::string& key,
                                        uint64_t start_ts,
                                        uint64_t end_ts);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIODeviceClientChannel__) */
