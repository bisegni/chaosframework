//
//  ChannelCache.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/24/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/global.h>
#include <chaos/common/data/cache/ChannelCache.h>
#include <boost/interprocess/detail/atomic.hpp>

using namespace chaos::data::cache;

ChannelCache::ChannelCache(memory::ManagedMemory *_memoryPool):readIndex(1),writeIndex(0),memoryPool(_memoryPool) {
    
    //clear the array
    rwPtr[0] = rwPtr[1] = NULL;
}

ChannelCache::~ChannelCache() {
}

//! set the readable index of the ptr array
void ChannelCache::swapRWIndex() {
    //swap read and write
    readIndex.store(writeIndex, boost::memory_order_release);
    
        //compute new write index
    writeIndex = (writeIndex + 1 % 2);
    
    // put old readeable slba into garbageable index
    garbageableSlab.push_back(rwPtr[writeIndex]);
    
        // alloc new slab info
    rwPtr[writeIndex] = makeNewChachedInfoPtr();
}

inline SlbCachedInfoPtr ChannelCache::makeNewChachedInfoPtr() {
    SlbCachedInfoPtr result = static_cast<SlbCachedInfoPtr>(memoryPool->allocate(slabRequiredSize, slabID));
    if(!result) return NULL;
        //clear all memory
    memset(result, 0, slabRequiredSize);
    result->references = 1;
    result->valPtr = result+sizeof(boost::uint32_t);
    return result;
}

void ChannelCache::garbageCache() {
    volatile boost::uint32_t *mem;
    boost::uint32_t oldMem, oldValue;
    
    //cicle all slab to make it garbaged
    for (std::vector<SlbCachedInfoPtr>::iterator iter = garbageableSlab.begin();
         iter != garbageableSlab.end();
         iter++) {
        
        SlbCachedInfoPtr tmpPtr = *iter;
        if(tmpPtr->references==1) {
            //try to put at 0 and garbage
            mem = &tmpPtr->references;
            oldMem = *mem;
            //increment the value with cas operation
            oldValue = boost::interprocess::ipcdetail::atomic_cas32(mem, *mem - 1, oldMem);
            
            if(oldValue == oldMem) {
                
                //lock the critical section
                boost::unique_lock<boost::mutex> lock(garbageMutext);
                
                //remove element
                garbageableSlab.erase(iter);
                
                //i can garbage the slab
                memoryPool->deallocate(tmpPtr, slabRequiredSize, slabID);
                
                lock.unlock();
            }
        }
        
    }
}

//! Initialize the channel cache
void ChannelCache::initChannel(const char *name, chaos::DataType::DataType type, uint32_t maxLength) {
    CHAOS_ASSERT(memoryPool)
    switch (channelType=type) {
        case chaos::DataType::TYPE_BOOLEAN:
            channelMaxLength = sizeof(char);
            break;
        case chaos::DataType::TYPE_DOUBLE:
            channelMaxLength = sizeof(double);
            break;
        case chaos::DataType::TYPE_INT32:
            channelMaxLength = sizeof(int32_t);
            break;
        case chaos::DataType::TYPE_INT64:
            channelMaxLength = sizeof(int64_t);
            break;
        case chaos::DataType::TYPE_STRING:
        case chaos::DataType::TYPE_BYTEARRAY:
            channelMaxLength = maxLength;
            break;
    }
    
    slabRequiredSize = (uint32_t)sizeof(SlbCachedInfo) + channelMaxLength;
    
        // retrive the rigth slab class info
    slabID = memoryPool->getSlabIdBySize(slabRequiredSize);
    
        //write the default writebla slab, clear all and call swapRWIndex to make it readable
    rwPtr[writeIndex] = makeNewChachedInfoPtr();
    rwPtr[readIndex.load(boost::memory_order_consume)] = makeNewChachedInfoPtr();
}

//! Set (and cache) the new value of the channel
void ChannelCache::updateValue(const void* channelValue, uint32_t valueLegth) {
    SlbCachedInfoPtr tmpPtr = rwPtr[writeIndex];
    uint32_t length = 0;
    switch (channelType) {
        case chaos::DataType::TYPE_BOOLEAN:
        case chaos::DataType::TYPE_DOUBLE:
        case chaos::DataType::TYPE_INT32:
        case chaos::DataType::TYPE_INT64:
            length = channelMaxLength;
            break;
        case chaos::DataType::TYPE_STRING:
        case chaos::DataType::TYPE_BYTEARRAY:
                //fix to remain into the maximum length
            length = min(valueLegth, channelMaxLength);
            break;
    }
        //copy the value into cache
    memcpy(tmpPtr->valPtr, channelValue, valueLegth);
}

SlbCachedInfoPtr ChannelCache::getCurrentCachedPtr() {
    SlbCachedInfoPtr result = NULL;
    volatile boost::uint32_t *mem;
    boost::uint32_t oldMem, oldValue;
    
        //get the old reference count from current RPtr
    do {
            //get info ptr
        result  = rwPtr[readIndex.load(boost::memory_order_consume)];
            //get ref pointer
        mem = &result->references;
            //if 0 is not usable
        if(*mem == 0) continue;
            //get the old value
        oldMem = *mem;
            //increment the value with cas operation
        oldValue = boost::interprocess::ipcdetail::atomic_cas32(mem, *mem + 1, oldMem);
        
            //check if old value is the same of the one memorized early
    } while (oldValue != oldMem);
    
        //we have suceed to udpate the reference count without noone has modified it
    return result;
}