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

#include <chaos/common/global.h>
#include <chaos/common/data/cache/LFDataCache.h>
#include <boost/interprocess/detail/atomic.hpp>

using namespace chaos::common::data::cache;


LFDataCache::LFDataCache(::chaos::memory::ManagedMemory *_memoryPool):
writeIndex(0),
readIndex(1),
memoryPool(_memoryPool),
garbageableSlab(1),
slabClassForCachedInfo(0),
maxLength(0),
slabRequiredSize(0),
slabID(0) {
    
    //clear the array
    rwPtr[0] = rwPtr[1] = NULL;
}

LFDataCache::~LFDataCache() {
}

//! set the readable index of the ptr array
void LFDataCache::swapRWIndex() {
    //swap read and write
    readIndex.store(writeIndex, boost::memory_order_release);
    
    //compute new write index
    writeIndex ^= 0x00000001;

    // put old readeable slba into garbageable index
    garbageableSlab.push(rwPtr[writeIndex]);
    
    // alloc new slab info
    rwPtr[writeIndex] = makeNewChachedInfoPtr();
}

inline SlbCachedInfoPtr LFDataCache::makeNewChachedInfoPtr() {
    SlbCachedInfoPtr result = static_cast<SlbCachedInfoPtr>(memoryPool->allocate(slabRequiredSize, slabID));
    if(!result) return NULL;
    //clear all memory
    //memset(result, 0, slabRequiredSize);
    result->references = 1;
    //result->valPtr = (SlbCachedInfoPtr)((char*)result+sizeof(boost::uint32_t));
    result->valPtr = (void*)((char*)result+sizeof(SlbCachedInfo));
    return result;
}

void LFDataCache::garbageCache() {
    int counter = 0;
    bool needToBeGarbaged = false;
    volatile boost::uint32_t *mem;
	boost::uint32_t oldMem = 0;
	boost::uint32_t oldValue = 0;

    
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
void LFDataCache::init(const char *name, uint32_t maxLength) {
    CHAOS_ASSERT(memoryPool)
    this->maxLength = maxLength;
    uint32_t structSize = (uint32_t)sizeof(SlbCachedInfo);
    slabRequiredSize = structSize + maxLength;
    
    // retrive the rigth slab class info
    slabID = memoryPool->getSlabIdBySize(slabRequiredSize);
    
    //write the default writebla slab, clear all and call swapRWIndex to make it readable
    rwPtr[0] = makeNewChachedInfoPtr();
    rwPtr[1] = makeNewChachedInfoPtr();
}

//! Set (and cache) the new value of the channel
void LFDataCache::updateValue(const void* value, uint32_t legth) {
    SlbCachedInfoPtr tmpPtr = rwPtr[writeIndex];
    //copy the value into cache
    
    memcpy(tmpPtr->valPtr, value, legth==0?maxLength:legth);
    
    //swap the wPtr with rPtr
    swapRWIndex();
}

SlbCachedInfoPtr LFDataCache::getCurrentCachedPtr() {
    SlbCachedInfoPtr result = NULL;
    volatile boost::uint32_t *mem;
    boost::uint32_t oldMem, oldValue;
    
    //get the old reference count from current RPtr
    //TODO reiimplemetne method with more modern code
//    do {
//        //get info ptr
//        result  = rwPtr[readIndex.load(boost::memory_order_consume)];
//        //get ref pointer
//        mem = &result->references;
//        //get the old value
//        oldMem = *mem;
//        //if 0 is not usable
//        if(oldMem == 0) continue;
//        //increment the value with cas operation
//        oldValue = boost::interprocess::ipcdetail::atomic_cas32(mem, *mem + 1, oldMem);
//
//        //check if old value is the same of the one memorized early
//    } while (oldValue != oldMem);
    
    //we have suceed to udpate the reference count without noone has modified it
    return result;
}

void LFDataCache::fillAccessorWithCurrentValue(ChannelValueAccessor&  accessorPtr) {
    accessorPtr.reset(getCurrentCachedPtr());
}
