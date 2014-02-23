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

#include "dataservice_global.h"
#include "AnswerEngine.h"
#include "cache_system/cache_system.h"
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;
namespace chaos{
    namespace data_service {
        
        class ChaosDataService;
        
        class DataConsumer : public DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler, public utility::StartableService {
            friend class ChaosDataService;
			std::string cache_impl_name;
			
			AnswerEngine				*answer_engine;
			ChaosDataServiceSetting		*settings;
            DirectIOServerEndpoint		*server_endpoint;
			DirectIODeviceServerChannel *device_channel;
			cache_system::CacheDriver	*cache_driver_instance;
			
            void consumeCDataWrapper(uint8_t channel_opcode, chaos::common::data::CDataWrapper *data_wrapper);
			void consumeDeviceEvent(opcode::DeviceChannelOpcode channel_opcode, void *channel_header_ptr, void *channel_data);
        public:
			DataConsumer();
            ~DataConsumer();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);

        };
    }
}

#endif /* defined(__CHAOSFramework__DataConsumer__) */
