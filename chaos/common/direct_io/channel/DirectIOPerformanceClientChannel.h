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
				DECLARE_CLASS_FACTORY(DirectIOPerformanceClientChannel, DirectIOVirtualClientChannel) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOPerformanceClientChannel)
					friend class DirectIOClientConnection;
					
					class DirectIOPerformanceClientChannelDeallocator:
					public DirectIODeallocationHandler {
					protected:
						void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr);
					};
					//static deallocator forthis channel
					static DirectIOPerformanceClientChannelDeallocator STATIC_DirectIOPerformanceClientChannelDeallocator;
					
					DirectIOPerformanceClientChannel(std::string alias);
					~DirectIOPerformanceClientChannel();
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
