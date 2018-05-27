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
                        virtual int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                                                     chaos::common::data::BufferSPtr& output_data)
                        {return -1;};
                        
                        //! Return the dataset for a producerkey ona specific snapshot
                        /*!
                         \param header of the snapshot where to fetch the dataasets
                         \param producer_id is the identification of the producre of the returning datasets
                         \return error on the forwading of the event
                         */
                        virtual int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader& header,
                                                                   const std::string& producer_id,
                                                                   chaos::common::data::BufferSPtr& channel_found_data,
                                                                   uint32_t& channel_found_data_length,
                                                                   DirectIOSystemAPISnapshotResultHeader &result_header)
                        {return -1;};
                        
                        //! Persist log entries for emitted by a node
                        /*!
                         \param header of the snapshot where to fetch the dataasets
                         \param producer_id is the identification of the producre of the returning datasets
                         \return error on the forwading of the event
                         */
                        virtual int consumeLogEntries(const std::string& node_name,
                                                      const ChaosStringVector& log_entries)
                        {return -1;};
                    } DirectIOSystemAPIServerChannelHandler;
                    
                    void setHandler(DirectIOSystemAPIServerChannelHandler *_handler);
                protected:
                    //! Handler for the event
                    DirectIOSystemAPIServerChannelHandler *handler;
                    
                    //!default constructor
                    DirectIOSystemAPIServerChannel(std::string alias);
                    
                    //! endpoint entry method
                    int consumeDataPack(DirectIODataPackSPtr data_pack,
                                        DirectIODataPackSPtr& synchronous_answer);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOSystemAPIServerChannel__) */
