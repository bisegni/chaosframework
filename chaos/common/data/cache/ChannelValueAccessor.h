//
//  ChannelValueAccessor.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 5/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ChannelValueAccessor__
#define __CHAOSFramework__ChannelValueAccessor__

#include <chaos/common/data/cache/SlbCachedInfo.h>

namespace chaos {
    namespace common {
		namespace data {
			
			namespace cache {
				
				class LFDataCache;
				
				class ChannelValueAccessor {
					friend class LFDataCache;
					
					SlbCachedInfoPtr slbPtr;
					
					
				public:
					ChannelValueAccessor();
					~ChannelValueAccessor();
					
					void reset(SlbCachedInfoPtr _slbPtr);
					
					template<typename T>
					T* getValuePtr() {
						return static_cast<T*>(slbPtr->valPtr);
					}
				};
				
				typedef ChannelValueAccessor *ChannelValueAccessorPtr;
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__ChannelValueAccessor__) */
