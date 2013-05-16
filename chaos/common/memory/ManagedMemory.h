/*
 *	ManagedMemory.h
 *	!CHOAS
 *	Create by Memcached team
 *  Update by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__ManagedMemory__
#define __CHAOSFramework__ManagedMemory__

#include <unistd.h>
#include <pthread.h>
#include <boost/atomic/atomic.hpp>
#include <boost/thread.hpp>
namespace chaos {
    
    namespace memory {
    
        /* Slab sizing definitions. */
        #define POWER_SMALLEST 0
        #define POWER_LARGEST  200
        #define CHUNK_ALIGN_BYTES 8
        #define DONT_PREALLOC_SLABS
        #define MAX_NUMBER_OF_SLAB_CLASSES (POWER_LARGEST + 1)
        
        /*! powers-of-N allocation structures */
        
        typedef struct {
            unsigned int size;      /*! sizes of items */
            unsigned int perslab;   /*! how many items per slab */
            
            void **slots;           /*! list of item ptrs */
            unsigned int sl_total;  /*! size of previous array */
            unsigned int sl_curr;   /*! first free slot */
            
            void *end_page_ptr;         /*! pointer to next free item at end of page, or 0 */
            unsigned int end_page_free; /*! number of items remaining at end of last alloced page */
            
            unsigned int slabs;     /*! how many slabs were allocated for this class */
            
            void **slab_list;       /*! array of slab pointers */
            unsigned int list_size; /*! size of prev array */
            
            unsigned int killing;  /*! index+1 of dying slab, or zero if none */
            size_t requested; /*! The number of requested bytes */
        } slabclass_t;
        
        //! Memcached memory slab managment
        /*!
         * Slabs memory allocation, based on powers-of-N. Slabs are up to 1MB in size
         * and are divided into chunks. The chunk sizes start off at the size of the
         * "item" structure plus space for a small key and value. They increase by
         * a multiplier factor from there, up to half the maximum slab size. The last
         * slab size is always 1MB, since that's the maximum item size allowed by the
         * memcached protocol.
         */
        class ManagedMemory {
            boost::atomic_flag atomicFlagLock;
            boost::shared_mutex accessMutex;
            slabclass_t slabclass[MAX_NUMBER_OF_SLAB_CLASSES];
            size_t mem_malloced;
            int power_largest;
            
            void *mem_base;
            void *mem_current;
            size_t mem_avail;
            
            int chunk_size;
            size_t item_size_max;
            size_t memoryLimit;
            double growFactor;
            int prealloc;
            /**
             * Access to the slab allocator is protected by this lock
             */
            pthread_mutex_t slabs_lock;
            
            inline int nz_strcmp(int nzlength, const char *nz, const char *z);
            inline void *memory_allocate(size_t size);
            inline int grow_slab_list (const unsigned int id);
            inline int do_slabs_newslab(const unsigned int id);
            inline void *do_slabs_alloc(const size_t size, unsigned int id);
            inline void do_slabs_free(void *ptr, const size_t size, unsigned int id);
        public:
            
            ManagedMemory();
            ~ManagedMemory();
            
                //! Inizialization of slab engine
            /*! Init the slab memory engine
             \param _chunkSize  is the minimal number of byte for the slab 
             \param _itemMaxSize is the maximum size in byte of the slab
             \param _memoryLimit if > 0 will put a barrier to the max memory to use
             \param _growFactor argument is the growth factor; each slab will use a chunk
                                size equal to the previous slab's chunk size times this factor.
             \param _prealloc allocate all mermoy for all slab (if 1) or allocate memory in chunks as it is needed (if 0)
             \param _fixedNumberOfSlab if > 0 give a fixed number of slba to every class
             */
            void init(int _chunkSize, size_t _itemMaxSize, size_t _memoryLimit, double _growFactor, int _prealloc, int _fixedNumberOfSlab = 0);
            
                //! Inizialization of slab engine
            /*! Init the slab memory engine for only one class of slab
             \param _chunkSize  is the minimal number of byte for the slab
             \param _memoryLimit if > 0 will put a barrier to the max memory to use
             \param _prealloc allocate all mermoy for all slab (if 1) or allocate memory in chunks as it is needed (if 0)
             \param _fixedNumberOfSlab if > 0 give a fixed number of slba to every class
             */
            void init(int _chunkSize, size_t _memoryLimit, int fixedNumberOfSlabPerClass, int _prealloc);
            
            void deinit();
            
            /**
             * Given object size, return id to use when allocating/freeing memory for object
             * 0 means error: can't store such a large object
             */
            unsigned int getSlabIdBySize(const size_t size);
            
            /** Allocate object of given length. 0 on error */ /*@null@*/
            void *allocate(const size_t size, unsigned int id);
            
            /** Free previously allocated object */
            void deallocate(void *ptr, size_t size, unsigned int id);
        };
        
    }
}

#endif /* defined(__CHAOSFramework__ManagedMemory__) */
