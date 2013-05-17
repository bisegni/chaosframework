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

ChannelCache::ChannelCache(memory::ManagedMemory *_memoryPool):readIndex(1),writeIndex(0),memoryPool(_memoryPool),garbageableSlab(1000) {
    
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
    writeIndex ^= 0x00000001;

    // put old readeable slba into garbageable index
    garbageableSlab.push(rwPtr[writeIndex]);
    
    // alloc new slab info
    rwPtr[writeIndex] = makeNewChachedInfoPtr();
}

inline SlbCachedInfoPtr ChannelCache::makeNewChachedInfoPtr() {
    SlbCachedInfoPtr result = static_cast<SlbCachedInfoPtr>(memoryPool->allocate(slabRequiredSize, slabID));
    if(!result) return NULL;
    //clear all memory
    //memset(result, 0, slabRequiredSize);
    result->references = 1;
    //result->valPtr = (SlbCachedInfoPtr)((char*)result+sizeof(boost::uint32_t));
    result->valPtr = (void*)((char*)result+sizeof(SlbCachedInfo));
    return result;
}

void ChannelCache::garbageCache() {
    int counter = 0;
    bool needToBeGarbaged = false;
    volatile boost::uint32_t *mem;
    boost::uint32_t oldMem, oldValue;

    
    if(garbageableSlab.empty()) return;
    
    //cicle all slab to make it garbaged
    SlbCachedInfoPtr tmpPtr = NULL;
    do {
        if((needToBeGarbaged=garbageableSlab.pop(tmpPtr))){
            counter++;
            if(tmpPtr->references==1) {
                //try to put at 0 and garbage
                mem = &tmpPtr->references;
                oldMem = *mem;
                //increment the value with cas operation
                oldValue = boost::interprocess::ipcdetail::atomic_cas32(mem, *mem - 1, oldMem);
                
                if(oldValue == oldMem) {
                    //i can garbage the slab
                    memoryPool->deallocate(tmpPtr, slabRequiredSize, slabID);
                }
            }
        }
    } while(needToBeGarbaged);
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
    uint32_t structSize = (uint32_t)sizeof(SlbCachedInfo);
    slabRequiredSize = structSize + channelMaxLength;
    
    // retrive the rigth slab class info
    slabID = memoryPool->getSlabIdBySize(slabRequiredSize);
    
    //write the default writebla slab, clear all and call swapRWIndex to make it readable
    rwPtr[0] = makeNewChachedInfoPtr();
    rwPtr[1] = makeNewChachedInfoPtr();
}

//! Set (and cache) the new value of the channel
void ChannelCache::updateValue(const void* channelValue, uint32_t valueLegth) {
    SlbCachedInfoPtr tmpPtr = rwPtr[writeIndex];
    //copy the value into cache
    
    memcpy(tmpPtr->valPtr, channelValue, valueLegth==0?channelMaxLength:valueLegth);
    
    //swap the wPtr with rPtr
    swapRWIndex();
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

void ChannelCache::fillAccessorWithCurrentValue(ChannelValueAccessor&  accessorPtr) {
    accessorPtr.reset(getCurrentCachedPtr());
}
