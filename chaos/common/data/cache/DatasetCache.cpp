    //
    //  DatasetCache.cpp
    //  CHAOSFramework
    //
    //  Created by Claudio Bisegni on 4/21/13.
    //  Copyright (c) 2013 INFN. All rights reserved.
    //

#include <chaos/common/data/cache/DatasetCache.h>

using namespace chaos::data::cache;

/*
 atomic<int> a(0);
 complex_data_structure data[2];
 
 thread1:
 data[1] = ...;
 a.store(1, memory_order_release);
 
 thread2:
 int index = a.load(memory_order_consume);
 complex_data_structure tmp = data[index];
 */

DatasetCache::DatasetCache() {
    chCachePtrArray = NULL;
}

DatasetCache::~DatasetCache() {
    
    
        //delete all channel cache creation
    if(chCachePtrArray) {
            //cicle all channel cache element of the array
        for (int idx = 0; idx < channelInfoMap.size(); idx++) {
                //delete channel cache
            delete(chCachePtrArray[idx]);
        }
        
            //delete empty cache array
        delete[] chCachePtrArray;
    }
    
        //deallocate map info
    for (std::map<std::string, ChannelInfo*>::iterator iter = channelInfoMap.begin();
         iter != channelInfoMap.end();
         iter++) {
            //delete the info
        delete(iter->second);
    }
    channelInfoMap.clear();
}

    //---------------------------------------------------------------

    //---------------------------------------------------------------

void DatasetCache::init(void* initParam) throw(chaos::CException) {
    ChannelCache *tmpChannelCache = NULL;
    
        //configure the cache using the maximum and minimum channel size
    memoryPool.init(minimumChannelSize+(uint32_t)sizeof(SlbCachedInfo), maximumChannelSize+(uint32_t)sizeof(SlbCachedInfo), 0, 1.2, 0, 1000);
    
        //allocate space for channel cache array
    chCachePtrArray = new ChannelCache*[channelInfoMap.size()];
    if(!chCachePtrArray)
        throw CException(1, "Error allocating channel cache array","DatasetCache::init");
    
        //clear all memory
    memset(chCachePtrArray, 0, sizeof(ChannelCache*)*channelInfoMap.size());
    
        //allocate alla channel cache
    for (std::map<std::string, ChannelInfo*>::iterator iter = channelInfoMap.begin();
         iter != channelInfoMap.end();
         iter++) {
        
        tmpChannelCache = new ChannelCache(&memoryPool);
        if(!tmpChannelCache) throw CException(2, "Error allocating channel cache","DatasetCache::init");
        
            //inizialize channel cache
        tmpChannelCache->initChannel(iter->first.c_str(), iter->second->type, iter->second->dimension);
        
            //allocate the channel cache array into his index
        chCachePtrArray[iter->second->index] = tmpChannelCache;
    }
}


    //---------------------------------------------------------------

    //---------------------------------------------------------------
void DatasetCache::start() throw(chaos::CException) {
}

    //---------------------------------------------------------------

    //---------------------------------------------------------------
void DatasetCache::stop() throw(chaos::CException) {
}

    //---------------------------------------------------------------

    //---------------------------------------------------------------
void DatasetCache::deinit() throw(chaos::CException) {
    memoryPool.deinit();
}

//---------------------------------------------------------------

//---------------------------------------------------------------
void DatasetCache::garbageCache() {
    for (std::map<std::string, ChannelInfo*>::iterator iter = channelInfoMap.begin();
         iter != channelInfoMap.end();
         iter++) {
        
        //allocate the channel cache array into his index
        chCachePtrArray[iter->second->index]->garbageCache();
    }
}
    //---------------------------------------------------------------

    //---------------------------------------------------------------
int DatasetCache::addChannel(const char * channelName,  chaos::DataType::DataType type, uint32_t channelDimension) {
    int err = 0;
        //check if we can receive other new channel
    if(getISDState() != chaos::utility::ISDInterface::ISD_DEINTIATED)
        throw CException(1, "Operation not permited, the cache is not initialized","DatasetCache::addChannel");
    
        //check if is already present into the hash
    if(channelInfoMap.count(channelName)) return 1;
    
        // insert a new channel
    ChannelInfo *chInfo = new ChannelInfo();
    if(!chInfo)
        throw CException(2, "Channel infor creation error","DatasetCache::addChannel");
    
    chInfo->index = (uint)channelInfoMap.size();
    
    switch (chInfo->type=type) {
        case chaos::DataType::TYPE_BOOLEAN:
            chInfo->dimension = sizeof(char);
            break;
        case chaos::DataType::TYPE_DOUBLE:
            chInfo->dimension = sizeof(double);
            break;
        case chaos::DataType::TYPE_INT32:
            chInfo->dimension = sizeof(int32_t);
            break;
        case chaos::DataType::TYPE_INT64:
            chInfo->dimension = sizeof(int64_t);
            break;
        case chaos::DataType::TYPE_STRING:
        case chaos::DataType::TYPE_BYTEARRAY:
            chInfo->dimension = channelDimension;
            break;
    }
    channelInfoMap.insert(make_pair(channelName, chInfo));
    
        //check the size and memorize the mAximum and minimum size
    maximumChannelSize = std::max(chInfo->dimension, maximumChannelSize);
    minimumChannelSize = std::min(chInfo->dimension, (minimumChannelSize==0)?maximumChannelSize:minimumChannelSize);
    return err;
}

void DatasetCache::updateChannelValue(const char *channelName, const void* channelValue, uint32_t valueLegth) {
    if(!channelInfoMap.count(channelName)) return;
    updateChannelValue(channelInfoMap[channelName]->index, channelValue, valueLegth);
}

void DatasetCache::updateChannelValue(uint16_t channelIndex, const void* channelValue, uint32_t valueLegth) {
    ChannelCache *ptr = chCachePtrArray[channelIndex];
    if(!ptr) return;
    ptr->updateValue(channelValue, valueLegth);
}

SlbCachedInfoPtr DatasetCache::getCurrentChannelCachedValue(const char *channelName) {
    if(!channelInfoMap.count(channelName)) return NULL;
    return getCurrentChannelCachedValue(channelInfoMap[channelName]->index);
}

SlbCachedInfoPtr DatasetCache::getCurrentChannelCachedValue(uint16_t channelIndex) {
    ChannelCache *ptr = chCachePtrArray[channelIndex];
    if(!ptr) return NULL;
    return ptr->getCurrentCachedPtr();
}

void DatasetCache::getCurrentChannelValueAccessor(const char *channelName, ChannelValueAccessor& accessorPtr) {
    if(!channelInfoMap.count(channelName)) {
        accessorPtr.reset(NULL);
        return;
    }
    getCurrentChannelValueAccessor(channelInfoMap[channelName]->index, accessorPtr);
}

void DatasetCache::getCurrentChannelValueAccessor(uint16_t channelIndex, ChannelValueAccessor& accessorPtr) {
    ChannelCache *ptr = chCachePtrArray[channelIndex];
    if(!ptr) {
        accessorPtr.reset(NULL);
        return;
    }
    ptr->fillAccessorWithCurrentValue(accessorPtr);
}