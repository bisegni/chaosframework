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
#include <chaos/common/memory/ManagedMemory.h>
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

using namespace chaos;
using namespace chaos::memory;

#define MM_LAPP_ LAPP_ << "[ManagedMemory] - "
#define MM_LERR_ LERR_ << "[ERROR:ManagedMemory] - "

ManagedMemory::ManagedMemory() {
    mem_malloced = 0;
    power_largest = 0;
    mem_base = NULL;
    mem_current = NULL;
    mem_avail = 0;
    chunk_size = 0;
    item_size_max = 0;
    memoryLimit = 0;
    growFactor = 0;
    prealloc = 0;
}

ManagedMemory::~ManagedMemory() {
    deinit();
}

/*
 * Figures out which slab class (chunk size) is required to store an item of
 * a given size.
 *
 * Given object size, return id to use when allocating/freeing memory for object
 * 0 means error: can't store such a large object
 */

unsigned int ManagedMemory::getSlabIdBySize(const size_t size) {
    int res = POWER_SMALLEST;
    
    if (size == 0)
        return -1;
    while (size > slabclass[res].size)
        if (res++ == power_largest)     /* won't fit in the biggest slab */
            return -1;
    return res;
}

/**
 * Determines the chunk sizes and initializes the slab class descriptors
 * accordingly.
 */
void ManagedMemory::init(int _chunkSize, size_t _itemMaxSize, size_t _memoryLimit, double _growFactor, int _prealloc, int _fixedNumberOfSlab) {
    //setup parameter
    chunk_size = _chunkSize;
    item_size_max = _itemMaxSize;
    memoryLimit = _memoryLimit;
    growFactor = _growFactor;
    prealloc = _prealloc >= 1 ?1:0;
    bool useFixedSlabNumber = _fixedNumberOfSlab != 0;
    
    int i = POWER_SMALLEST - 1;
    unsigned int size = chunk_size;
    size_t calculated_all_slabs_mem =0;
    MM_LAPP_ <<  "usign chunk size:" << chunk_size;
    
    memset(slabclass, 0, sizeof(slabclass));
    
    
    
    while (++i < POWER_LARGEST && size <= item_size_max / growFactor) {
        /* Make sure items are always n-byte aligned */
        if (size % (unsigned int)CHUNK_ALIGN_BYTES) size += (unsigned int)CHUNK_ALIGN_BYTES - (size % (unsigned int)CHUNK_ALIGN_BYTES);
        
        slabclass[i].size = size;
        slabclass[i].perslab = useFixedSlabNumber ? _fixedNumberOfSlab:(unsigned int)(item_size_max / slabclass[i].size);
        
        calculated_all_slabs_mem += slabclass[i].size*slabclass[i].perslab;
        
        size = (unsigned int)(size * growFactor);
        MM_LAPP_ <<  "slab class: "<< i <<" chunk size:" << slabclass[i].size << " perslab:" << slabclass[i].perslab;
    }
    
    power_largest = i;
    slabclass[power_largest].size = (unsigned int)item_size_max;
    slabclass[power_largest].perslab = useFixedSlabNumber ? _fixedNumberOfSlab:1;
    calculated_all_slabs_mem += slabclass[power_largest].size*slabclass[power_largest].perslab;
    MM_LAPP_ <<  "slab class: "<< i <<" chunk size:" << slabclass[i].size << " perslab:" << slabclass[i].perslab;
    
    /* Allocate everything in a big chunk with malloc */
    if(memoryLimit <= 0) {
        //allocate all memoryneeded by all calculated slabs
        memoryLimit = calculated_all_slabs_mem;
    }
    
    mem_base = std::malloc(memoryLimit);
    if (mem_base != NULL) {
        mem_current = mem_base;
        mem_avail = memoryLimit;
    } else {
        MM_LERR_ <<  "Warning: Failed to allocate requested memory in one large chunk.Will allocate in smaller chunks";
    }
    
    if(prealloc) {
        for (i = POWER_SMALLEST; i <= POWER_LARGEST; i++) {
            if (++prealloc > power_largest)
                    return;
            do_slabs_newslab(i);
        }
    }
}

void ManagedMemory::init(int _chunkSize, size_t _memoryLimit, int fixedNumberOfSlabPerClass, int _prealloc) {
    bool slabFixedNumber = fixedNumberOfSlabPerClass!=0;
    chunk_size = _chunkSize;
    item_size_max = 0;
    memoryLimit = _memoryLimit;
    growFactor = 1;
    prealloc = _prealloc >= 1 ?1:0;
    
    unsigned int size = chunk_size;
    size_t calculated_all_slabs_mem =0;
    MM_LAPP_ <<  "usign chunk size:" << chunk_size;
    
    memset(slabclass, 0, sizeof(slabclass));
    
    /* Make sure items are always n-byte aligned */
    if (size % (unsigned int)CHUNK_ALIGN_BYTES) size += (unsigned int)CHUNK_ALIGN_BYTES - (size % (unsigned int)CHUNK_ALIGN_BYTES);
    
    slabclass[POWER_SMALLEST].size = size;
    slabclass[POWER_SMALLEST].perslab = slabFixedNumber?fixedNumberOfSlabPerClass:(unsigned int)(item_size_max / slabclass[POWER_SMALLEST].size);
    
    calculated_all_slabs_mem += slabclass[POWER_SMALLEST].size*slabclass[POWER_SMALLEST].perslab;
    
    size = (unsigned int)(size * growFactor);
    MM_LAPP_ <<  "slab class: "<< POWER_SMALLEST <<" chunk size:" << slabclass[POWER_SMALLEST].size << " perslab:" << slabclass[POWER_SMALLEST].perslab;
    /* Allocate everything in a big chunk with malloc */
    if(memoryLimit <= 0) {
        //allocate all memoryneeded by all calculated slabs
        memoryLimit = calculated_all_slabs_mem;
    }
    
    mem_base = std::malloc(memoryLimit);
    if (mem_base != NULL) {
        mem_current = mem_base;
        mem_avail = memoryLimit;
    } else {
        MM_LERR_ <<  "Warning: Failed to allocate requested memory in one large chunk.Will allocate in smaller chunks";
    }
    
    if(prealloc) {
        do_slabs_newslab(POWER_SMALLEST);
    }

}

void ManagedMemory::deinit() {
    //deallocate all slab
    if(mem_base) {
        std::free(mem_base);
        mem_base = NULL;
    }
}

void *ManagedMemory::allocate(size_t size, unsigned int id) {
    void *ret = NULL;
    boost::lock_guard<boost::shared_mutex> lock(accessMutex);
    //while (atomicFlagLock.test_and_set(boost::memory_order_acquire)) {};
    ret = do_slabs_alloc(size, id);
    //atomicFlagLock.clear(boost::memory_order_release);
    return ret;
}

void ManagedMemory::deallocate(void *ptr, size_t size, unsigned int id) {
    boost::lock_guard<boost::shared_mutex> lock(accessMutex);
    //while (atomicFlagLock.test_and_set(boost::memory_order_acquire)) {};
    do_slabs_free(ptr, size, id);
    //atomicFlagLock.clear(boost::memory_order_release);
}

//----------------- private --------------

int ManagedMemory::grow_slab_list(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    if (p->slabs == p->list_size) {
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void *new_list = realloc(p->slab_list, new_size * sizeof(void *));
        if (new_list == 0) return 0;
        p->list_size = (unsigned int)new_size;
        p->slab_list = (void **)new_list;
    }
    return 1;
}

int ManagedMemory::do_slabs_newslab(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    int len = p->size * p->perslab;
    char *ptr;
    
    if ((memoryLimit && mem_malloced + len > memoryLimit && p->slabs > 0) ||
        (grow_slab_list(id) == 0) ||
        ((ptr = (char*)memory_allocate((size_t)len)) == 0)) {
        
        return 0;
    }
    
    memset(ptr, 0, (size_t)len);
    p->end_page_ptr = ptr;
    p->end_page_free = p->perslab;
    
    p->slab_list[p->slabs++] = ptr;
    mem_malloced += len;
    
    return 1;
}


void *ManagedMemory::do_slabs_alloc(const size_t size, unsigned int id) {
    slabclass_t *p;
    void *ret = NULL;
    
    if (id < POWER_SMALLEST || id > power_largest) {
        return NULL;
    }
    
    p = &slabclass[id];
    
    /* fail unless we have space at the end of a recently allocated page,
     we have something on our freelist, or we could allocate a new page */
    if (! (p->end_page_ptr != 0 || p->sl_curr != 0 ||
           do_slabs_newslab(id) != 0)) {
        /* We don't have more memory available */
        ret = NULL;
    } else if (p->sl_curr != 0) {
        /* return off our freelist */
        ret = p->slots[--p->sl_curr];
    } else {
        /* if we recently allocated a whole page, return from that */
        assert(p->end_page_ptr != NULL);
        ret = p->end_page_ptr;
        if (--p->end_page_free != 0) {
            p->end_page_ptr = ((caddr_t)p->end_page_ptr) + p->size;
        } else {
            p->end_page_ptr = 0;
        }
    }
    
    if (ret) {
        p->requested += size;
    }
    
    return ret;
}

void ManagedMemory::do_slabs_free(void *ptr, const size_t size, unsigned int id) {
    slabclass_t *p;
    assert(id >= POWER_SMALLEST && id <= power_largest);
    if (id < POWER_SMALLEST || id > power_largest)
        return;
    
    p = &slabclass[id];
    
    if (p->sl_curr == p->sl_total) { /* need more space on the free list */
        int new_size = (p->sl_total != 0) ? p->sl_total * 2 : 16;  /* 16 is arbitrary */
        void **new_slots = (void**)realloc(p->slots, new_size * sizeof(void *));
        if (new_slots == 0)
            return;
        p->slots = new_slots;
        p->sl_total = new_size;
    }
    p->slots[p->sl_curr++] = ptr;
    p->requested -= size;
    return;
}

int ManagedMemory::nz_strcmp(int nzlength, const char *nz, const char *z) {
    size_t zlength=strlen(z);
    return (zlength == nzlength) && (strncmp(nz, z, zlength) == 0) ? 0 : -1;
}

void *ManagedMemory::memory_allocate(size_t size) {
    void *ret;
    
    if (mem_base == NULL) {
        /* We are not using a preallocated large memory chunk */
        ret = malloc(size);
    } else {
        ret = mem_current;
        
        if (size > mem_avail) {
            return NULL;
        }
        
        /* mem_current pointer _must_ be aligned!!! */
        if (size % CHUNK_ALIGN_BYTES) {
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        }
        
        mem_current = ((char*)mem_current) + size;
        if (size < mem_avail) {
            mem_avail -= size;
        } else {
            mem_avail = 0;
        }
    }
    
    return ret;
}