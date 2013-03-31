//
//  FastCache.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/31/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__FastCache__
#define __CHAOSFramework__FastCache__

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <chaos/common/data/cache/CacheGlobal.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/memory/ManagedMemory.h>

namespace chaos {
    namespace data {
        namespace cache {
            
            using namespace chaos::memory;
            
            typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
            typedef  unsigned       char ub1;   /* unsigned 1-byte quantities */
            /** Maximum length of a key. */
            #define hashsize(n) ((ub4)1<<(n))
            #define hashmask(n) (hashsize(n)-1)
            

            
            #define LARGEST_ID POWER_LARGEST

            
            class FastCache : public FastHash, memory::ManagedMemory {
                
                /* current time of day (updated periodically) */
                volatile rel_time_t current_time;
                
                /* how many powers of 2's worth of buckets we use */
                unsigned int hashpower;
                
                /* Main hash table. This is where we look except during expansion. */
                item** primary_hashtable;
                
                /*
                 * Previous hash table. During expansion, we look here for keys that haven't
                 * been moved over to the primary yet.
                 */
                item** old_hashtable;
                
                /* Number of items in the hash table. */
                unsigned int hash_items;
                
                /* Flag: Are we in the middle of expanding now? */
                int expanding;
                /*
                 * During expansion we migrate values with bucket granularity; this is how
                 * far we've gotten so far. Ranges from 0 .. hashsize(hashpower - 1) - 1.
                 */
                unsigned int expand_bucket;
                volatile int do_run_maintenance_thread;
                

                int hash_bulk_move;
                
                pthread_t maintenance_tid;
                pthread_cond_t maintenance_cond;
                pthread_mutex_t mc_cache_lock;
                
                CacheSettings settings;
                
                item *heads[LARGEST_ID];
                item *tails[LARGEST_ID];
                itemstats_t itemstats[LARGEST_ID];
                unsigned int sizes[LARGEST_ID];
                
            protected:
                void item_link_q(item *it);
                void item_unlink_q(item *it);
                
                static void* maintenance_thread(void *arg);
                
                item** _hashitem_before(const char *key, const size_t nkey);
                
                /* grows the hashtable to the next power of 2. */
                void assoc_expand(void);
                //void* assoc_maintenance_thread(void *arg);
            public:
                FastCache(int _chunkSize, size_t _itemMaxSize, size_t _memoryLimit, double _growFactor, int _prealloc);
                void assoc_init();
                item *assoc_find(const char *key, const size_t nkey);
                int assoc_insert(item *item);
                void assoc_delete(const char *key, const size_t nkey);
                void do_assoc_move_next_bucket();
                int start_assoc_maintenance_thread();
                void stop_assoc_maintenance_thread();
                
                uint64_t get_cas_id(void);
                size_t item_make_header(const uint8_t nkey, const int flags, const int nbytes,
                                                   char *suffix, uint8_t *nsuffix);
                item *do_item_alloc(const char *key, const size_t nkey, const int flags, const rel_time_t exptime, const int nbytes);
                void item_free(item *it);
                int item_size_ok(const size_t nkey, const int flags, const int nbytes);
                
                int  do_item_link(item *it);     /** may fail if transgresses limits */
                void do_item_unlink(item *it);
                void do_item_remove(item *it);
                void do_item_update(item *it);   /** update LRU time to current and reposition */
                int  do_item_replace(item *it, item *new_it);
                
                /*@null@*/
                char *do_item_cachedump(const unsigned int slabs_clsid, const unsigned int limit, unsigned int *bytes);
                
                void do_item_flush_expired(void);
                
                item *do_item_get(const char *key, const size_t nkey);
                item *do_item_get_nocheck(const char *key, const size_t nkey);
                void item_stats_reset(void);
                /**
                 * Create an object cache.
                 *
                 * The object cache will let you allocate objects of the same size. It is fully
                 * MT safe, so you may allocate objects from multiple threads without having to
                 * do any syncrhonization in the application code.
                 *
                 * @param name the name of the object cache. This name may be used for debug purposes
                 *             and may help you track down what kind of object you have problems with
                 *             (buffer overruns, leakage etc)
                 * @param bufsize the size of each object in the cache
                 * @param align the alignment requirements of the objects in the cache.
                 * @param constructor the function to be called to initialize memory when we need
                 *                    to allocate more memory from the os.
                 * @param destructor the function to be called before we release the memory back
                 *                   to the os.
                 * @return a handle to an object cache if successful, NULL otherwise.
                 */
                cache_t* cache_create(const char* name, size_t bufsize, size_t align,
                                      cache_constructor_t* constructor,
                                      cache_destructor_t* destructor);
                /**
                 * Destroy an object cache.
                 *
                 * Destroy and invalidate an object cache. You should return all buffers allocated
                 * with cache_alloc by using cache_free before calling this function. Not doing
                 * so results in undefined behavior (the buffers may or may not be invalidated)
                 *
                 * @param handle the handle to the object cache to destroy.
                 */
                void cache_destroy(cache_t* handle);
                /**
                 * Allocate an object from the cache.
                 *
                 * @param handle the handle to the object cache to allocate from
                 * @return a pointer to an initialized object from the cache, or NULL if
                 *         the allocation cannot be satisfied.
                 */
                void* cache_alloc(cache_t* handle);
                /**
                 * Return an object back to the cache.
                 *
                 * The caller should return the object in an initialized state so that
                 * the object may be returned in an expected state from cache_alloc.
                 *
                 * @param handle handle to the object cache to return the object to
                 * @param ptr pointer to the object to return.
                 */
                void cache_free(cache_t* handle, void* ptr);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__FastCache__) */
