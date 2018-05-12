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
					
					
                    int consumeDataPack(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
				};
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__DirectIOPerformanceServerChannel__) */
