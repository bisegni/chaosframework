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
#include <boost/lockfree/queue.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/thread.hpp>

#include <chaos/common/cconstants.h>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/SlbCachedInfo.h>
#include <chaos/common/data/cache/ChannelValueAccessor.h>

namespace chaos {
    
    namespace data {
        
        namespace cache {
            
                //forward declaration
            class DatasetCache;
            
            
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
                
                    //!slab id to use
                uint16_t slabID;
                
                    //! the type of the channel
                chaos::DataType::DataType channelType;
                
                    //! used slab array cache slab
                boost::lockfree::queue<SlbCachedInfoPtr, boost::lockfree::fixed_sized<false> > garbageableSlab;
                
                //! set the readable index of the ptr array
                /*!
                 atomically se the index to the value to use for get the
                 readable slab info
                 */
                inline void swapRWIndex();
                
                //! allocate and prepare a new slab
                /*!
                 Allocate a new slab and prepare it to be used
                 \return a new SlbCachedInfo structure to cache
                         a new valu efor the channel
                 */
                inline SlbCachedInfoPtr makeNewChachedInfoPtr();
            public:
                
                    //! Default constructor
                /*!
                 Creathe the object with the memory manager assigned
                 */
                ChannelCache(memory::ManagedMemory *_memoryPool);
                
                    //! Default destructor
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
                
                    //! return the current cached accessor
                /*!
                 */
                void fillAccessorWithCurrentValue(ChannelValueAccessor&  accessorPtr);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ChannelCache__) */
