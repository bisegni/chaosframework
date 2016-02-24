//
//  DataCache.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/1/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DataCache__
#define __CHAOSFramework__DataCache__

#include <stdint.h>
#include <unistd.h>
//#include <pthread.h>
#include <chaos/common/data/cache/CacheGlobal.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/StartableService.h>
#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace data {
			namespace cache {
				
				using namespace chaos::memory;
				
				typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
				typedef  unsigned       char ub1;   /* unsigned 1-byte quantities */
				/** Maximum length of a key. */
				
#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)
				
#define LARGEST_ID POWER_LARGEST
				
				//! Memcached mebedded implementation
				/*!
				 This class is the embedded version of memcached, all code for networking has been keeped out.
				 */
				class DataCache : private FastHash, private memory::ManagedMemory, protected utility::StartableService  {
					
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
					
					
					item *heads[LARGEST_ID];
					item *tails[LARGEST_ID];
					itemstats_t itemstats[LARGEST_ID];
					unsigned int sizes[LARGEST_ID];
					
					void item_link_q(item *it);
					void item_unlink_q(item *it);
					
					static void* maintenance_thread(void *arg);
					
					item** _hashitem_before(const char *key, const size_t nkey);
					
					
					/* grows the hashtable to the next power of 2. */
					void assoc_expand(void);
					void assoc_init();
					int assoc_insert(item *item);
					void assoc_delete(const char *key, const size_t nkey);
					void do_assoc_move_next_bucket();
					int start_assoc_maintenance_thread();
					void stop_assoc_maintenance_thread();
					
					uint64_t get_cas_id(void);
					size_t item_make_header(const uint8_t nkey, const int flags, const int nbytes,
											char *suffix, uint8_t *nsuffix);
					void item_free(item *it);
					int item_size_ok(const size_t nkey, const int flags, const int nbytes);
					item *do_item_get_nocheck(const char *key, const size_t nkey);
					void item_stats_reset(void);
				protected:
					CacheSettings settings;
					boost::shared_mutex mc_mutex;
					pthread_t maintenance_tid;
					pthread_cond_t maintenance_cond;
					pthread_mutex_t mc_cache_lock;
					
					inline int  do_item_link(item *it);     /** may fail if transgresses limits */
					inline void do_item_unlink(item *it);
					inline void do_item_remove(item *it);
					inline int  do_item_replace(item *it, item *new_it);
					inline item *assoc_find(const char *key, const size_t nkey);
					inline item *do_item_alloc(const char *key, const size_t nkey, const int flags, const rel_time_t exptime, const int nbytes);
					inline item *do_item_get(const char *key, const size_t nkey);
					
				public:
					
					//!Constructor
					DataCache();
					
					//!Destructor
					virtual ~DataCache();
					
					//! Initialize instance
					void init(void* initParam) throw(chaos::CException);
					
					//! Start the implementation
					void start() throw(chaos::CException);
					
					//! Start the implementation
					void stop() throw(chaos::CException);
					
					//! Deinit the implementation
					void deinit() throw(chaos::CException);
					
					//! get item
					virtual int getItem(const char *key, uint32_t& buffLen, void *returnBuffer);
					
					//! store item
					virtual int storeItem(const char *key, const void *buffer, uint32_t bufferLen);
					
					//! delete item
					virtual int deleteItem(const char *key);
					
					//! check if an item is present
					virtual bool isItemPresent(const char *key);
				};
				
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__DataCache__) */
