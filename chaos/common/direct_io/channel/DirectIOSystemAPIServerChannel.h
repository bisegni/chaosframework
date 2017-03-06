/*
 *	DirectIOPerformanceServerChannel.h
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
                DECLARE_CLASS_FACTORY(DirectIOSystemAPIServerChannel, DirectIOVirtualServerChannel),
                public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOSystemAPIServerChannel)
                    
                    class DirectIOSystemAPIServerChannelDeallocator:
                    public DirectIODeallocationHandler {
                    protected:
                        void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr);
                    };
                    //static deallocator forthis channel
                    static DirectIOSystemAPIServerChannelDeallocator STATIC_DirectIOSystemAPIServerChannelDeallocator;
                    
                public:
                    //! System API DirectIO server handler
                    typedef class DirectIOSystemAPIServerChannelHandler {
                    public:
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
                                                                   DirectIOSystemAPISnapshotResultHeader &result_header)
                        {DELETE_HEADER(header) return 0;};
                        
                        //! Persist log entries for emitted by a node
                        /*!
                         \param header of the snapshot where to fetch the dataasets
                         \param producer_id is the identification of the producre of the returning datasets
                         \return error on the forwading of the event
                         */
                        virtual int consumeLogEntries(const std::string& node_name,
                                                      const ChaosStringVector& log_entries)
                        {return 0;};
                    } DirectIOSystemAPIServerChannelHandler;
                    
                    void setHandler(DirectIOSystemAPIServerChannelHandler *_handler);
                protected:
                    //! Handler for the event
                    DirectIOSystemAPIServerChannelHandler *handler;
                    
                    //!default constructor
                    DirectIOSystemAPIServerChannel(std::string alias);
                    
                    //! endpoint entry method
                    int consumeDataPack(DirectIODataPack *dataPack,
                                        DirectIODataPack *synchronous_answer,
                                        DirectIODeallocationHandler **answer_header_deallocation_handler,
                                        DirectIODeallocationHandler **answer_data_deallocation_handler);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOSystemAPIServerChannel__) */
