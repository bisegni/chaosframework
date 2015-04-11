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

#ifndef __CHAOSFramework__DirectIOPerformanceServerChannel__
#define __CHAOSFramework__DirectIOPerformanceServerChannel__

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
				
				DECLARE_CLASS_FACTORY(DirectIOPerformanceServerChannel, DirectIOVirtualServerChannel), public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOPerformanceServerChannel)
					friend class DirectIODispatcher;
					
					DirectIOPerformanceServerChannel(std::string alias);
					~DirectIOPerformanceServerChannel();
				public:
					//! Device handler definition
					class DirectIOPerformanceServerChannelHandler {
                    public:
						//! handle a request to perform a roundtrip test
						virtual void handleReqRoundTripRequest(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request) = 0;
						
						//! andle the response to an round trip test request
						virtual void handleRespRoundTripRequest(opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request) = 0;
					};
					
					
                    void setHandler(DirectIOPerformanceServerChannelHandler *_handler);
				protected:
					//! Handler for the event
					DirectIOPerformanceServerChannelHandler *handler;
					
					
					int consumeDataPack(DirectIODataPack *dataPack, DirectIOSynchronousAnswerPtr synchronous_answer);
				};
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__DirectIOPerformanceServerChannel__) */
