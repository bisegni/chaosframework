/*
 *	DataConsumer.cpp
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

#ifndef __CHAOSFramework__DataConsumer__
#define __CHAOSFramework__DataConsumer__
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIOCDataWrapperServerChannel.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

namespace chaos{
    namespace data_service {
        
        class ChaosDataService;
        
        class DataConsumer : public DirectIOCDataWrapperServerChannel::DirectIOCDataWrapperServerChannelHandler, public utility::StartableService {
            friend class ChaosDataService;
            DirectIOServerEndpoint *server_endpoint;
            DirectIOCDataWrapperServerChannel *server_channel;
			boost::thread_group client_threads_group;
			uint32_t	received;
			uint32_t	last_received;
			uint64_t	last_received_ts;
			uint32_t	sent;
			uint32_t	last_sent;
			uint64_t	last_sent_ts;
            uint32_t    last_seq;
			chaos::TimingUtil timing_util;
			
			void simulateClient(DirectIOClient *client_instance);
            void consumeCDataWrapper(uint8_t channel_tag, chaos::common::data::CDataWrapper *data_wrapper);
        public:
			DataConsumer();
            ~DataConsumer();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);
			
			void addClient(DirectIOClient *client);
        };
    }
}

#endif /* defined(__CHAOSFramework__DataConsumer__) */
