//
//  DirectIOEndpointHandler.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 07/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIOEndpointHandler_h
#define CHAOSFramework_DirectIOEndpointHandler_h

#include <chaos/common/direct_io/DirectIODataPack.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOEndpointHandler {
			public:
				virtual void consumeDataPack(DirectIODataPack *dataPack) = 0;
			};
		}
	}
}

#endif
