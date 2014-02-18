//
//  CacheGlobal.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/31/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_CacheGlobal_h
#define CHAOSFramework_CacheGlobal_h
namespace chaos {
	namespace common {
		namespace data {
			namespace cache {
				typedef  unsigned       int  rel_time_t;
				
				
				/*
				 * We only reposition items in the LRU queue if they haven't been repositioned
				 * in this many seconds. That saves us from churning on frequently-accessed
				 * items.
				 */
#define ITEM_UPDATE_INTERVAL 60
				
#define KEY_MAX_LENGTH 250
#define ITEM_LINKED 1
#define ITEM_CAS 2
#define ITEM_SLABBED 4
#define DEFAULT_HASH_BULK_MOVE 1
				
				/* warning: don't use these macros with a function, as it evals its arg twice */
#define ITEM_get_cas(i) (((i)->it_flags & ITEM_CAS) ? \
(i)->data->cas : (uint64_t)0)
				
#define ITEM_set_cas(i,v) { \
if ((i)->it_flags & ITEM_CAS) { \
(i)->data->cas = v; \
} \
}
				
#define ITEM_key(item) (((char*)&((item)->data)) \
+ (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))
				
#define ITEM_suffix(item) ((char*) &((item)->data) + (item)->nkey + 1 \
+ (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))
				
#define ITEM_data(item) ((char*) &((item)->data) + (item)->nkey + 1 \
+ (item)->nsuffix \
+ (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))
				
#define ITEM_ntotal(item) (sizeof(struct _stritem) + (item)->nkey + 1 \
+ (item)->nsuffix + (item)->nbytes \
+ (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))
				
				/**
				 * Structure for storing items within memcached.
				 */
				typedef struct _stritem {
					struct _stritem *next;
					struct _stritem *prev;
					struct _stritem *h_next;    /* hash chain next */
					int             nbytes;     /* size of data */
					unsigned short  refcount;
					uint8_t         nsuffix;    /* length of flags-and-length string */
					uint8_t         it_flags;   /* ITEM_* above */
					uint8_t         slabs_clsid;/* which slab class we're in */
					uint8_t         nkey;       /* key length, w/terminating null and padding */
					/* this odd type prevents type-punning issues when we do
					 * the little shuffle to save space when not using CAS. */
					union {
						uint64_t cas;
						char end;
					} data[];
					/* if it_flags & ITEM_CAS we have 8 bytes CAS */
					/* then null-terminated key */
					/* then " flags length\r\n" (no terminating null) */
					/* then data with terminating \r\n (no terminating null; it's binary!) */
				} item;
				
				typedef struct settings {
					size_t maxbytes;
					rel_time_t oldest_live; /* ignore existing items older than this */
					int evict_to_free;
					double factor;          /* chunk size growth factor */
					int chunk_size;
					int use_cas;
					int item_size_max;        /* Maximum item size, and upper end for slabs */
					int preallocation;
				} CacheSettings;
				
				
				typedef struct {
					unsigned int evicted;
					unsigned int evicted_nonzero;
					rel_time_t evicted_time;
					unsigned int reclaimed;
					unsigned int outofmemory;
					unsigned int tailrepairs;
				} itemstats_t;
			}
		}
    }
}
#endif
