//
//  CacheGlobal.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/31/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_CacheGlobal_h
#define CHAOSFramework_CacheGlobal_h

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
 * Constructor used to initialize allocated objects
 *
 * @param obj pointer to the object to initialized.
 * @param notused1 This parameter is currently not used.
 * @param notused2 This parameter is currently not used.
 * @return you should return 0, but currently this is not checked
 */
typedef int cache_constructor_t(void* obj, void* notused1, int notused2);
/**
 * Destructor used to clean up allocated objects before they are
 * returned to the operating system.
 *
 * @param obj pointer to the object to initialized.
 * @param notused1 This parameter is currently not used.
 * @param notused2 This parameter is currently not used.
 * @return you should return 0, but currently this is not checked
 */
typedef void cache_destructor_t(void* obj, void* notused);

/**
 * Structure for storing items within memcached.
 */
typedef struct _stritem {
    struct _stritem *next;
    struct _stritem *prev;
    struct _stritem *h_next;    /* hash chain next */
    rel_time_t      time;       /* least recent access */
    rel_time_t      exptime;    /* expire time */
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


/**
 * Definition of the structure to keep track of the internal details of
 * the cache allocator. Touching any of these variables results in
 * undefined behavior.
 */
typedef struct {
    /** Mutex to protect access to the structure */
    pthread_mutex_t mutex;
    /** Name of the cache objects in this cache (provided by the caller) */
    char *name;
    /** List of pointers to available buffers in this cache */
    void **ptr;
    /** The size of each element in this cache */
    size_t bufsize;
    /** The capacity of the list of elements */
    int freetotal;
    /** The current number of free elements */
    int freecurr;
    /** The constructor to be called each time we allocate more memory */
    cache_constructor_t* constructor;
    /** The destructor to be called each time before we release memory */
    cache_destructor_t* destructor;
} cache_t;

typedef struct settings {
    size_t maxbytes;
    rel_time_t oldest_live; /* ignore existing items older than this */
    int evict_to_free;
    double factor;          /* chunk size growth factor */
    int chunk_size;
    int use_cas;
    int backlog;
    int item_size_max;        /* Maximum item size, and upper end for slabs */
    int sasl;              /* SASL on/off */
} CacheSettings;


typedef struct {
    unsigned int evicted;
    unsigned int evicted_nonzero;
    rel_time_t evicted_time;
    unsigned int reclaimed;
    unsigned int outofmemory;
    unsigned int tailrepairs;
} itemstats_t;

#endif
