/*
 *	DirectIODeviceClientChannel.h
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
#ifndef __CHAOSFramework__DirectIODeviceClientChannel__
#define __CHAOSFramework__DirectIODeviceClientChannel__

#include <string>
#include <stdint.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos_data = chaos::common::data;
namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				//! Class for the managment of pushing data for the device dataset
				/*!
				 This class manage the forwarding of data that represent the device dataset channels (i/O)
				 */
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIODeviceClientChannel, DirectIOVirtualClientChannel) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceClientChannel)
					
					uint32_t device_hash;
					std::string device_id;

				protected:
					DirectIODeviceClientChannel(std::string alias);
				public:
					~DirectIODeviceClientChannel();
					
					//! Set the device id for this channel
					void setDeviceID(std::string _device_id);
					
					//! Send device serialization with priority
                    int64_t putDataOutputChannel(bool cache_it, void *buffer, uint32_t buffer_len);
					
					//! Send a request for the last output data
                    int64_t requestLastOutputData(uint16_t server_port, uint16_t endpoint_idx);
				};

				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIODeviceClientChannel__) */
