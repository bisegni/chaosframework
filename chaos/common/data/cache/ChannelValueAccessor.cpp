//
//  ChannelValueAccessor.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 5/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//


#include <boost/interprocess/detail/atomic.hpp>

#include <chaos/common/data/cache/ChannelValueAccessor.h>

using namespace chaos::data::cache;

ChannelValueAccessor::ChannelValueAccessor():slbPtr(NULL) {
    
}

ChannelValueAccessor::~ChannelValueAccessor() {
    reset(NULL);
}

void ChannelValueAccessor::reset(SlbCachedInfoPtr _slbPtr) {
    if(slbPtr) {
        boost::interprocess::ipcdetail::atomic_add32(&slbPtr->references,  (boost::uint32_t)-1);
    }
    
    slbPtr = _slbPtr;
}