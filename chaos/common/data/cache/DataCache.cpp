//
//  DataCache.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/31/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <chaos/common/data/cache/DataCache.h>


/** How long an object can reasonably be assumed to be locked before
 harvesting it on a low memory condition. */
#define TAIL_REPAIR_TIME (3 * 3600)

using namespace chaos::common::data::cache;

const int initial_pool_size = 64;

DataCache::DataCache(){
    hashpower = 16;
    primary_hashtable = NULL;
    old_hashtable = NULL;
    hash_items = 0;
    expanding = 0;
    expand_bucket = 0;
    do_run_maintenance_thread = 1;
    hash_bulk_move = DEFAULT_HASH_BULK_MOVE;
    
    memset(&settings, 0, sizeof(CacheSettings));
}

DataCache::~DataCache() {
    
}

//! Initialize instance
void DataCache::init(void* initParam) throw(chaos::CException) {
    CacheSettings *cp = initParam ? static_cast<CacheSettings*>(initParam):NULL;
    if(!cp) throw CException(1, "Error retriving init parameter", "DataCache::init");
    memcpy(&settings, cp, sizeof(CacheSettings));
        
    memory::ManagedMemory::init(settings.chunk_size, settings.item_size_max, settings.maxbytes, settings.factor, settings.preallocation);
    
    //initialize memcached association logic
    assoc_init();
}


//! Start the implementation
void DataCache::start() throw(chaos::CException) {
    
   // if( start_assoc_maintenance_thread() ) {
     //   throw CException(-1, "start_assoc_maintenance_thread", "FastCache::start");
   // }
}

//! Start the implementation
void DataCache::stop() throw(chaos::CException) {
    //stop_assoc_maintenance_thread();
}

//! Deinit the implementation
void DataCache::deinit() throw(chaos::CException) {
    memory::ManagedMemory::deinit();
}


//! get item
int DataCache::getItem(const char *key, uint32_t& buffLen, void *returnBuffer) {
    item *it = NULL;
    boost::unique_lock<boost::shared_mutex> lock(mc_mutex);
    it = do_item_get(key, strlen(key));
    if (!it) {
        return -1;
    }
    /* Add the data minus the CRLF */
    buffLen = it->nbytes;
    memcpy(returnBuffer, ITEM_data(it), buffLen);
    do_item_remove(it);
    return 0;
}


//! store item
int DataCache::storeItem(const char *key, const void *buffer, uint32_t bufferLen) {
    boost::unique_lock<boost::shared_mutex> lock(mc_mutex);
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
    
    if (old_it != NULL) {
        //do_item_replace(old_it, new_it);
        assert((old_it->it_flags & ITEM_SLABBED) == 0);
        do_item_unlink(old_it);
        do_item_link(new_it);
    } else
        do_item_link(new_it);

    
    if (old_it != NULL)
        do_item_remove(old_it);
    if (new_it != NULL)
        do_item_remove(new_it);
    return 0;
}

//! delete item
int DataCache::deleteItem(const char *key) {
    boost::unique_lock<boost::shared_mutex>(mc_mutex);
    item *it = do_item_get(key, strlen(key));
    if(!it) {
        return -1;
    }
    do_item_unlink(it);
    do_item_remove(it);
    return 0;
}

//! check if an item is present
bool DataCache::isItemPresent(const char *key) {
    item *it = do_item_get(key, strlen(key));
    return it !=NULL;
}
#pragma GCC visibility push(hidden)

void DataCache::assoc_init(void) {
    primary_hashtable = (item**)calloc(hashsize(hashpower), sizeof(item**));
    if (! primary_hashtable) {
        exit(EXIT_FAILURE);
    }
}

item *DataCache::assoc_find(const char *key, const size_t nkey) {
    uint32_t hv = hash(key, nkey, 0);
    item *it;
    unsigned int oldbucket;
    
    if (expanding &&
        (oldbucket = (hv & hashmask(hashpower - 1))) >= expand_bucket)
    {
        it = old_hashtable[oldbucket];
    } else {
        it = primary_hashtable[hv & hashmask(hashpower)];
    }
    
    item *ret = NULL;
    int depth = 0;
    while (it) {
        if ((nkey == it->nkey) && (memcmp(key, ITEM_key(it), nkey) == 0)) {
            ret = it;
            break;
        }
        it = it->h_next;
        ++depth;
    }
    return ret;
}

/* returns the address of the item pointer before the key.  if *item == 0,
 the item wasn't found */

item** DataCache::_hashitem_before (const char *key, const size_t nkey) {
    uint32_t hv = hash(key, nkey, 0);
    item **pos;
    unsigned int oldbucket;
    
    if (expanding &&
        (oldbucket = (hv & hashmask(hashpower - 1))) >= expand_bucket)
    {
        pos = &old_hashtable[oldbucket];
    } else {
        pos = &primary_hashtable[hv & hashmask(hashpower)];
    }
    
    while (*pos && ((nkey != (*pos)->nkey) || memcmp(key, ITEM_key(*pos), nkey))) {
        pos = &(*pos)->h_next;
    }
    return pos;
}

/* grows the hashtable to the next power of 2. */
void DataCache::assoc_expand() {
    old_hashtable = primary_hashtable;
    
    primary_hashtable = (item**)calloc(hashsize(hashpower + 1), sizeof(void *));
    if (primary_hashtable) {
        hashpower++;
        expanding = 1;
        expand_bucket = 0;
        pthread_cond_signal(&maintenance_cond);
    } else {
        primary_hashtable = old_hashtable;
        /* Bad news, but we can keep running. */
    }
}

/* Note: this isn't an assoc_update.  The key must not already exist to call this */
int DataCache::assoc_insert(item *it) {
    uint32_t hv;
    unsigned int oldbucket;
    
    assert(assoc_find(ITEM_key(it), it->nkey) == 0);  /* shouldn't have duplicately named things defined */
    
    hv = hash(ITEM_key(it), it->nkey, 0);
    if (expanding &&
        (oldbucket = (hv & hashmask(hashpower - 1))) >= expand_bucket)
    {
        it->h_next = old_hashtable[oldbucket];
        old_hashtable[oldbucket] = it;
    } else {
        it->h_next = primary_hashtable[hv & hashmask(hashpower)];
        primary_hashtable[hv & hashmask(hashpower)] = it;
    }
    
    hash_items++;
    if (! expanding && hash_items > (hashsize(hashpower) * 3) / 2) {
        assoc_expand();
    }
    
    return 1;
}

void DataCache::assoc_delete(const char *key, const size_t nkey) {
    item **before = _hashitem_before(key, nkey);
    
    if (*before) {
        item *nxt;
        hash_items--;
        /* The DTrace probe cannot be triggered as the last instruction
         * due to possible tail-optimization by the compiler
         */
        nxt = (*before)->h_next;
        (*before)->h_next = 0;   /* probably pointless, but whatever. */
        *before = nxt;
        return;
    }
    /* Note:  we never actually get here.  the callers don't delete things
     they can't find. */
    assert(*before != 0);
}

void *DataCache::maintenance_thread(void *arg) {
    
    DataCache *fcObjPtr = static_cast<DataCache*>(arg);
    
    while (fcObjPtr->do_run_maintenance_thread) {
        int ii = 0;
        
        /* Lock the cache, and bulk move multiple buckets to the new
         * hash table. */
        boost::unique_lock<boost::shared_mutex>(fcObjPtr->mc_mutex);
        //pthread_mutex_lock(&fcObjPtr->mc_cache_lock);
        
        for (ii = 0; ii < fcObjPtr->hash_bulk_move && fcObjPtr->expanding; ++ii) {
            item *it, *next;
            int bucket;
            
            for (it = fcObjPtr->old_hashtable[fcObjPtr->expand_bucket]; NULL != it; it = next) {
                next = it->h_next;
                
                bucket = fcObjPtr->hash(ITEM_key(it), it->nkey, 0) & hashmask(fcObjPtr->hashpower);
                it->h_next = fcObjPtr->primary_hashtable[bucket];
                fcObjPtr->primary_hashtable[bucket] = it;
            }
            
            fcObjPtr->old_hashtable[fcObjPtr->expand_bucket] = NULL;
            
            fcObjPtr->expand_bucket++;
            if (fcObjPtr->expand_bucket == hashsize(fcObjPtr->hashpower - 1)) {
                fcObjPtr->expanding = 0;
                free(fcObjPtr->old_hashtable);
            }
        }
        
        if (!fcObjPtr->expanding) {
            /* We are done expanding.. just wait for next invocation */
            pthread_cond_wait(&fcObjPtr->maintenance_cond, &fcObjPtr->mc_cache_lock);
        }
        
        //pthread_mutex_unlock(&fcObjPtr->mc_cache_lock);
    }
    return NULL;
}

int DataCache::start_assoc_maintenance_thread() {
    int ret;
    char *env = getenv("MEMCACHED_HASH_BULK_MOVE");
    if (env != NULL) {
        hash_bulk_move = atoi(env);
        if (hash_bulk_move == 0) {
            hash_bulk_move = DEFAULT_HASH_BULK_MOVE;
        }
    }
    if ((ret = pthread_create(&maintenance_tid, NULL,
                              DataCache::maintenance_thread, this)) != 0) {
        return -1;
    }
    return 0;
}

void DataCache::stop_assoc_maintenance_thread() {
    boost::unique_lock<boost::mutex>(mc_mutex);
    do_run_maintenance_thread = 0;
    pthread_cond_signal(&maintenance_cond);
    
    /* Wait for the maintenance thread to stop */
    pthread_join(maintenance_tid, NULL);
}

/**
 * Generates the variable-sized part of the header for an object.
 *
 * key     - The key
 * nkey    - The length of the key
 * flags   - key flags
 * nbytes  - Number of bytes to hold value and addition CRLF terminator
 * suffix  - Buffer for the "VALUE" line suffix (flags, size).
 * nsuffix - The length of the suffix is stored here.
 *
 * Returns the total size of the header.
 */
size_t DataCache::item_make_header(const uint8_t nkey, const int flags, const int nbytes,
                                   char *suffix, uint8_t *nsuffix) {
    /* suffix is defined at 40 chars elsewhere.. */
    *nsuffix = (uint8_t) snprintf(suffix, 40, " %d %d\r\n", flags, nbytes - 2);
    return sizeof(item) + nkey + *nsuffix + nbytes;
}

/* Get the next CAS id for a new item. */
uint64_t DataCache::get_cas_id(void) {
    static uint64_t cas_id = 0;
    return ++cas_id;
}


/*@null@*/
item *DataCache::do_item_alloc(const char *key, const size_t nkey, const int flags, const rel_time_t exptime, const int nbytes) {
    uint8_t nsuffix;
    item *it = NULL;
    char suffix[40];
    size_t ntotal = item_make_header(nkey + 1, flags, nbytes, suffix, &nsuffix);
    if (settings.use_cas) {
        ntotal += sizeof(uint64_t);
    }
    
    unsigned int id = getSlabIdBySize(ntotal);
    if (id == 0)
        return 0;
    int tries = 50;
    
    if (it == NULL && (it = (item *)allocate(ntotal, id)) == NULL) {
        /*
         ** Could not find an expired item at the tail, and memory allocation
         ** failed. Try to evict some items!
         */
        tries = 50;
        
        /* If requested to not push old items out of cache when memory runs out,
         * we're out of luck at this point...
         */
        
        if (settings.evict_to_free == 0) {
            itemstats[id].outofmemory++;
            return NULL;
        }
        
        /*
         * try to get one off the right LRU
         * don't necessariuly unlink the tail because it may be locked: refcount>0
         * search up from tail an item with refcount==0 and unlink it; give up after 50
         * tries
         */
        
        if (tails[id] == 0) {
            itemstats[id].outofmemory++;
            return NULL;
        }
        
        it = (item *)allocate(ntotal, id);
    }
    
    assert(it->slabs_clsid == 0);
    
    it->slabs_clsid = id;
    
    assert(it != heads[it->slabs_clsid]);
    
    it->next = it->prev = it->h_next = 0;
    it->refcount = 1;     /* the caller will have a reference */
    it->it_flags = settings.use_cas ? ITEM_CAS : 0;
    it->nkey = nkey;
    it->nbytes = nbytes;
    memcpy(ITEM_key(it), key, nkey);
    memcpy(ITEM_suffix(it), suffix, (size_t)nsuffix);
    it->nsuffix = nsuffix;
    return it;
}

void DataCache::item_free(item *it) {
    size_t ntotal = ITEM_ntotal(it);
    unsigned int clsid;
    assert((it->it_flags & ITEM_LINKED) == 0);
    assert(it != heads[it->slabs_clsid]);
    assert(it != tails[it->slabs_clsid]);
    assert(it->refcount == 0);
    
    /* so slab size changer can tell later if item is already free or not */
    clsid = it->slabs_clsid;
    it->slabs_clsid = 0;
    it->it_flags |= ITEM_SLABBED;
    deallocate(it, ntotal, clsid);
}

/**
 * Returns 1 if an item will fit in the cache (its size does not exceed
 * the maximum for a cache entry.)
 */
int DataCache::item_size_ok(const size_t nkey, const int flags, const int nbytes) {
    char prefix[40];
    uint8_t nsuffix;
    
    size_t ntotal = item_make_header(nkey + 1, flags, nbytes,
                                     prefix, &nsuffix);
    if (settings.use_cas) {
        ntotal += sizeof(uint64_t);
    }
    
    return getSlabIdBySize(ntotal) != 0;
}

void DataCache::item_link_q(item *it) { /* item is the new head */
    item **head, **tail;
    assert(it->slabs_clsid < LARGEST_ID);
    assert((it->it_flags & ITEM_SLABBED) == 0);
    
    head = &heads[it->slabs_clsid];
    tail = &tails[it->slabs_clsid];
    assert(it != *head);
    assert((*head && *tail) || (*head == 0 && *tail == 0));
    it->prev = 0;
    it->next = *head;
    if (it->next) it->next->prev = it;
    *head = it;
    if (*tail == 0) *tail = it;
    sizes[it->slabs_clsid]++;
    return;
}

void DataCache::item_unlink_q(item *it) {
    item **head, **tail;
    assert(it->slabs_clsid < LARGEST_ID);
    head = &heads[it->slabs_clsid];
    tail = &tails[it->slabs_clsid];
    
    if (*head == it) {
        assert(it->prev == 0);
        *head = it->next;
    }
    if (*tail == it) {
        assert(it->next == 0);
        *tail = it->prev;
    }
    assert(it->next != it);
    assert(it->prev != it);
    
    if (it->next) it->next->prev = it->prev;
    if (it->prev) it->prev->next = it->next;
    sizes[it->slabs_clsid]--;
    return;
}

/** wrapper around assoc_find which does the lazy expiration logic */
item *DataCache::do_item_get(const char *key, const size_t nkey) {
    item *it = assoc_find(key, nkey);
    if (it != NULL) {
        it->refcount++;
    }
    return it;
}

/** returns an item whether or not it's expired. */
item *DataCache::do_item_get_nocheck(const char *key, const size_t nkey) {
    item *it = assoc_find(key, nkey);
    if (it) {
        it->refcount++;
    }
    return it;
}

int DataCache::do_item_link(item *it) {
    assert((it->it_flags & (ITEM_LINKED|ITEM_SLABBED)) == 0);
    it->it_flags |= ITEM_LINKED;
    assoc_insert(it);
    
    /* Allocate a new CAS ID on link. */
    ITEM_set_cas(it, (settings.use_cas) ? get_cas_id() : 0);
    
    item_link_q(it);
    
    return 1;
}

void DataCache::do_item_unlink(item *it) {
    if ((it->it_flags & ITEM_LINKED) != 0) {
        it->it_flags &= ~ITEM_LINKED;
        assoc_delete(ITEM_key(it), it->nkey);
        item_unlink_q(it);
        if (it->refcount == 0) item_free(it);
    }
}

void DataCache::do_item_remove(item *it) {
    assert((it->it_flags & ITEM_SLABBED) == 0);
    if (it->refcount != 0) {
        it->refcount--;
    }
    if (it->refcount == 0 && (it->it_flags & ITEM_LINKED) == 0) {
        item_free(it);
    }
}

int DataCache::do_item_replace(item *it, item *new_it) {
    assert((it->it_flags & ITEM_SLABBED) == 0);
    
    do_item_unlink(it);
    return do_item_link(new_it);
}

#pragma GCC visibility pop