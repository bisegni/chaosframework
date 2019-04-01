//
//  ChannelValueAccessor.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 5/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//


#include <boost/interprocess/detail/atomic.hpp>

#include <chaos/common/data/cache/ChannelValueAccessor.h>

using namespace chaos::common::data::cache;

ChannelValueAccessor::ChannelValueAccessor():slbPtr(NULL) {
    
}

ChannelValueAccessor::~ChannelValueAccessor() {
    reset(NULL);
}

void ChannelValueAccessor::reset(SlbCachedInfoPtr _slbPtr) {
    if(slbPtr) {
#ifdef _WIN32
		//boost::interprocess::ipcdetail::atomic_dec32(&slbPtr->references);
		boost::interprocess::ipcdetail::atomic_write32(&slbPtr->references, (boost::interprocess::ipcdetail::atomic_read32(&slbPtr->references) + (boost::uint32_t) - 1));
#else
        boost::interprocess::ipcdetail::atomic_add32(&slbPtr->references,  (boost::uint32_t)-1);
#endif
    }
    
    slbPtr = _slbPtr;
}