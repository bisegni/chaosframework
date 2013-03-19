//
//  DataCache.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/23/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//
#include <chaos/common/data/cache/memcached.h>
#include <chaos/common/data/cache/DataCache.h>

using namespace chaos;
using namespace chaos::datacache;

/*!
 */
DataCache::DataCache() {
    memset((void*)&settings, 0, sizeof(struct settings));
}

/*!
 */
DataCache::~DataCache() {
}


//! Initialize instance
void DataCache::init(void* initParam) throw(chaos::CException) {
    CacheInitParameter *cp = initParam ? static_cast<CacheInitParameter*>(initParam):NULL;
    
    settings.factor = (cp ? cp->factor : 1.25);
    settings.use_cas = 1;
    settings.maxbytes = (cp ? cp->maxByte : 4 * 1024 * 1024); //default is 4MB
    settings.chunk_size = (cp ? cp->chunkSize : 48); //48;         /* space for a modest key and value */
    settings.oldest_live = 0;
    settings.item_size_max = (cp ? cp->maxItemByte : 1024 * 1024); //1024 * 1024;
}

//! Start the implementation
void DataCache::start() throw(chaos::CException) {
    
    //initialize memcached association logic
    assoc_init();
    
    //initalize slab memory
    slabs_init(settings.maxbytes, settings.factor, 1);
    
    if( start_assoc_maintenance_thread() ) {
        throw CException(-1, "start_assoc_maintenance_thread", "DataCache::start");
    }
}

//! Start the implementation
void DataCache::stop() throw(chaos::CException) {
    
}

//! Deinit the implementation
void DataCache::deinit() throw(chaos::CException) {
    stop_assoc_maintenance_thread();
}

//! get item
int DataCache::getItem(const char *key, int32_t& buffLen, void **returnBuffer) {
    item *it = NULL;
    
    pthread_mutex_lock(&cache_lock);
    it = assoc_find(key, strlen(key));
    it->refcount++;
    pthread_mutex_unlock(&cache_lock);
    if (!it) {
         return -1;
    }
    
    /* Add the data minus the CRLF */
    buffLen = it->nbytes;
    *returnBuffer = ITEM_data(it);
    it->refcount--;

    return 0;
}


//! store item
int DataCache::storeItem(const char *key, const void *buffer, int32_t bufferLen) {
    item *old_it = do_item_get(key, strlen(key));
    
    item *new_it = do_item_alloc(key, strlen(key), 0, 0, bufferLen);
    if (new_it == NULL) {
        /* SERVER_ERROR out of memory */
        if (old_it != NULL) {
            do_item_remove(old_it);
        }
        return -1;
    }else{
        memcpy(ITEM_data(new_it), buffer, bufferLen);
    }
    
    pthread_mutex_lock(&cache_lock);
    if (old_it != NULL)
        do_item_replace(old_it, new_it);
    else
        do_item_link(new_it);
    pthread_mutex_unlock(&cache_lock);
    
    if (old_it != NULL)
        do_item_remove(old_it);
    if (new_it != NULL)
        do_item_remove(new_it);
    
    return 0;
}

//! delete item
int DataCache::deleteItem(const char *key) {
    item *it = do_item_get(key, strlen(key));
    if(!it) {
        return -1;
    }
    pthread_mutex_lock(&cache_lock);
    do_item_unlink(it);
    do_item_remove(it);
    pthread_mutex_unlock(&cache_lock);
    return 0;
}