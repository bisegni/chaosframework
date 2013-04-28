//
//  ChannelCache.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/24/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ChannelCache__
#define __CHAOSFramework__ChannelCache__

#include <vector>

#include <boost/atomic/atomic.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/thread.hpp>

#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/cconstants.h>

namespace chaos {
    
    namespace data {
        
        namespace cache {
            
                //forward declaration
            class DatasetCache;
            
            /*!
             Define the cached slab taht contain channel description
             */
            typedef struct SlbCachedInfo {
                    //! busy flag indication
                    /*!
                     It is used to up the reference count at the moment that
                     rptr is read by array. It possible that the rptr that 
                     is getting becam old and so the clean thread will try to clean
                     before the count is up of one.
                     Using this flag the thread can be hald in the moment 
                     */
                //boost::atomic_flag busyFlag;
                
                    //! element reference count
                /*!
                 Mantain the number of the object that refere to this element of the cache,
                 the default value for the reference is "1" when it is 0 it ca be deallocated
                 */
                boost::uint32_t references;
                
                    //!slab ptr for cached value
                /*!
                 This ptr is the base address where the value is wrote to be cached
                 */
                void *valPtr;
            } SlbCachedInfo, *SlbCachedInfoPtr;
            
            
                //!Channel cache
            /*!
             This class will manage the single channel cache. It work using a two position array
             that has the write and read ptr to the SlbCachedInfo. When new value is wrote, the old
             ReadPTR is remove and the index is swapped. this permit to promote the WritePtr as new Read Ptr
             and a new slab is create for permit the new value storage.
             */
            class ChannelCache {
                    //!permit the DatasetCache class to use the private variable
                friend class DatasetCache;
                
                boost::mutex garbageMutext;
                
                    //!index to identity the write ptr
                int writeIndex;
                
                    //! index for identify read ptr
                boost::atomic_ushort readIndex;
                
                //! writeble and readeble pointer array
                /*!
                 The two element array is used to select where, the principal thread write
                 the next "current" value, and the other is used for read the current value.
                 */
                SlbCachedInfoPtr rwPtr[2];
                
                    //!Slab memory pool
                memory::ManagedMemory *memoryPool;
                    //! the class of the slab that can contains this channel value
                int slabClassForCachedInfo;
                
                    //! the max length of the channel
                uint32_t channelMaxLength;
                
                    //! slab size
                uint32_t slabRequiredSize;
                
                uint16_t slabID;
                
                    //! the type of the channel
                chaos::DataType::DataType channelType;
                
                    //! used slab array cache slab
                std::vector<SlbCachedInfoPtr> garbageableSlab;
                
                //! set the readable index of the ptr array
                /*!
                 atomically se the index to the value to use for get the
                 readable slab info
                 */
                inline void swapRWIndex();
                
                inline SlbCachedInfoPtr makeNewChachedInfoPtr();
            public:
                ChannelCache(memory::ManagedMemory *_memoryPool);
                ~ChannelCache();
                
                /*!
                 Delete the unused cached slab
                 */
                void garbageCache();
                
                //! Initialize the channel cache
                /*!
                 Inizialize all the structure necassary to cache the serialized fragment for the 
                 channel value
                 */
                void initChannel(const char *name, chaos::DataType::DataType type, uint32_t maxLength = 0);
                
                //! Set (and cache) the new value of the channel
                /*!
                 Set the new value of the channel executing the operation descripted below:
                    - generate or update the serialized fragment for the current channel value,
                        into the writeable slab pointer;
                    - swap the writeable with the readeable pointer using the index array
                 */
                void updateValue(const void* channelValue, uint32_t valueLegth = 0);
                
                    //! return the current cached memory
                /*!
                 */
                SlbCachedInfoPtr getCurrentCachedPtr();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ChannelCache__) */
