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
#ifndef __CHAOSFramework__DirectIOVirtualChannel__
#define __CHAOSFramework__DirectIOVirtualChannel__

#include <string>
#include <stdint.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/direct_io/channel/DirectIOChannelGlobal.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClient;
			
			namespace channel {
				class DirectIOVirtualChannel :
				public  utility::NamedService {
					friend class chaos::common::direct_io::DirectIOClient;
					
				protected:
					uint8_t channel_route_index;
					DirectIOVirtualChannel(std::string _channel_name, uint8_t _channel_route_index);
					virtual ~DirectIOVirtualChannel();
				public:
					uint8_t getChannelRouterIndex();
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
