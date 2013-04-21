//
//  Dataset.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/21/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/data/DatasetCache.h>

using namespace chaos::data::cache;

//---------------------------------------------------------------

//---------------------------------------------------------------

void Dataset::init(void* initParam) throw(chaos::CException) {
    //allocate the memory necessary into the cache;
    CacheSettings cacheSettings;
    
    //clear all settings
    std::memset(&cacheSettings, 0, sizeof(CacheSettings));

    //configure the cache information
    cacheSettings.factor = 1.2;     //grow factor
    cacheSettings.chunk_size = minimumChannelSize;     //minimum element size
    cacheSettings.item_size_max = maximumChannelSize;  //max item size

    DataCache::init(static_cast<void*>(&cacheSettings));
}


//---------------------------------------------------------------

//---------------------------------------------------------------
void Dataset::start() throw(chaos::CException) {
    DataCache::start();
}

//---------------------------------------------------------------

//---------------------------------------------------------------
void Dataset::stop() throw(chaos::CException) {
    DataCache::stop();
}

//---------------------------------------------------------------

//---------------------------------------------------------------
void Dataset::deinit() throw(chaos::CException) {
    DataCache::deinit();
}


//---------------------------------------------------------------

//---------------------------------------------------------------
int Dataset::addChannel(const char * channelName, uint32_t channelDimension) {
    int err = 0;
    //check if we can receive other new channel
    if(getISDState() != chaos::utility::ISDInterface::ISD_DEINTIATED)
        throw CException(1, "Operation not permited, the cache is not initialized","Dataset::addChannel");
    
    //check if is already present into the hash
    if(channelPostionMap.count(channelName)) return 1;
    
    // insert a new channel
    unsigned int newPosition = (unsigned int)channelPostionMap.size();
    channelPostionMap.insert(make_pair(channelName, ++newPosition));
    
    // add default value
    auto_ptr<unsigned char> tmpPtr(new unsigned char (channelDimension));
    memset(tmpPtr.get(), 0, channelDimension);
    
    err = DataCache::storeItem(channelName, tmpPtr.get(), channelDimension);

    if(err == 0) {
        //check the size and memorize the mximum and minimum size
        minimumChannelSize = std::min(minimumChannelSize, channelDimension);
        maximumChannelSize = std::max(maximumChannelSize, channelDimension);
    }
    return err;
}

//---------------------------------------------------------------

//---------------------------------------------------------------
int Dataset::deleteItem(const char *channelName) {
    //check if we can receive other new channel
    if(getISDState() != chaos::utility::ISDInterface::ISD_DEINTIATED)
        throw CException(1, "Operation not permited, the cache is not initialized","Dataset::addChannel");
    
    //check if is already present into the hash
    if(channelPostionMap.count(channelName)) return 1;
    
    channelPostionMap.erase(channelName);
    return DataCache::deleteItem(channelName);
}