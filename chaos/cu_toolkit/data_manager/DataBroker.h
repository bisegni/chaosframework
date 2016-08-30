/*
 *	DataBroker.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h
#define __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h

#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/publishing/publishing.h>
#include <chaos/cu_toolkit/data_manager/manipulation/manipulation.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system.h>

namespace chaos {
    namespace cu {
        namespace data_manager {

            
            //! main class for the data broker
            /*!
             DatBroker is a multilayer architecture that manage and publish
             dataset iwth thei attribute. Has also functionality for trigger algorithm on 
             determinated dataset's attribute.
             */
            class DataBroker:
            public chaos::common::utility::InizializableService,
            public chaos::common::direct_io::channel::DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler {
                //! is the node uid that host the databrker and will be sed as
                //! rpc domain
                const std::string owner_node_uid;
                //!root sublayer managment
                manipulation::DataBrokerEditor  dataset_manager;
                publishing::PublishingManager   publishing_manager;
            protected:
                //---------------- DirectIODeviceServerChannelHandler -----------------------
                int consumePutEvent(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
                                    void *channel_data,
                                    uint32_t channel_data_len);
                
                int consumeGetEvent(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
                                    void *channel_data,
                                    uint32_t channel_data_len,
                                    chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                    void **result_value);
                
                int consumeDataCloudQuery(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header,
                                          const std::string& search_key,
                                          uint64_t search_start_ts,
                                          uint64_t search_end_ts);
            public:
                DataBroker(const std::string& _owner_node_uid);
                ~DataBroker();
                
                void init(void* init_data) throw(CException);
                
                void deinit() throw(CException);
                
                //!return the dataset editor
                manipulation::DataBrokerEditor& getEditor();
            };
            
        }
    }
}

#endif /* __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h */
