//
//  SlbCachedInfo.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 5/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_SlbCachedInfo_h
#define CHAOSFramework_SlbCachedInfo_h

#include <boost/cstdint.hpp>
//#pragma pack(push, 4)
/*!
 Define the cached slab taht contain channel description
 */
typedef struct SlbCachedInfo {
    //! element reference count
    /*!
     Mantain the number of the object that refere to this element of the cache,
     the default value for the reference is "1" when it is 0 it ca be deallocated
     */
    boost::uint32_t references;
    
    //!slab ptr for cached value
    /*!
     This ptr is the base address where the value is wrote to be cached
     */
    void *valPtr;
} SlbCachedInfo, *SlbCachedInfoPtr;
//#pragma pack(pop)
#endif
