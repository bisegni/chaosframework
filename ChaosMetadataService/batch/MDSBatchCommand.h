/*
 *	BatchCommand.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__MDSBatchCommand__
#define __CHAOSFramework__MDSBatchCommand__

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/message/DeviceMessageChannel.h>

namespace chaos{
    namespace metadata_service {
        namespace batch {
            
            //forward declaration
            class MDSBatchExecutor;

            
            //! base class for all metadata service batch command
            /*!
             Represent the base class for all metadata server command. Every 
             command is na instance of this class ans o many egual command can be launched
             with different parameter
             */
            class MDSBatchCommand:
            public chaos::common::batch_command::BatchCommand {
                friend class MDSBatchExecutor;
                
                //!message channel for communitcation with other node
                chaos::common::network::NetworkBroker *network_broker;
            protected:
                //! default constructor
                MDSBatchCommand();
                
                //! default destructor
                ~MDSBatchCommand();
                
                //! return a raw message channel
                chaos::common::message::MessageChannel *getNewMessageChannel();

                    //! return a raw multinode message channel
                chaos::common::message::MultiAddressMessageChannel *getNewMultiAddressMessageChannel();

                //! return a device message channel for a specific node address
                chaos::common::message::DeviceMessageChannel *getNewDeviceMessageChannelForAddress(chaos::common::network::CDeviceNetworkAddress *device_network_address);
                
                // inherited method
                void releaseChannel(chaos::common::message::MessageChannel *message_channel);
                
                // return the implemented handler
                uint8_t implementedHandler();
                
                // inherited method
                void setHandler(chaos_data::CDataWrapper *data);
                
                // inherited method
                void acquireHandler();
                
                // inherited method
                void ccHandler();
                
                // inherited method
                bool timeoutHandler();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__MDSBatchCommand__) */
