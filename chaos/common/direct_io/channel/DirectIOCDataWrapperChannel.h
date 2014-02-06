//
//  DirectIOCDataWrapperChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DirectIOCDataWrapperChannel__
#define __CHAOSFramework__DirectIOCDataWrapperChannel__

#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOCDataWrapperChannel:protected DirectIOVirtualClientChannel, protected DirectIOVirtualClientChannel  {
				
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOCDataWrapperChannel__) */
