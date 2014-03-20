/*
 *	DirectIOPerformanceClientChannel.h
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

#ifndef __CHAOSFramework__DirectIOPerformanceClientChannel__
#define __CHAOSFramework__DirectIOPerformanceClientChannel__

#include <string>
#include <stdint.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClientConnection;
			namespace channel {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIOPerformanceClientChannel, DirectIOVirtualClientChannel) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOPerformanceClientChannel)
					friend class DirectIOClientConnection;
					
					DirectIOPerformanceClientChannel(std::string alias);
					~DirectIOPerformanceClientChannel();
				protected:
					void freeSentData(void *data,  DisposeSentMemoryInfo& dispose_memory_info);
				public:
					int64_t sendRoundTripMessage();
					int64_t answerRoundTripMessage(uint64_t received_ts);
					int64_t answerRoundTripMessage(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr received_header);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__PerformanceChannel__) */
