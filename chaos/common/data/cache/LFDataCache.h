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

#ifndef __CHAOSFramework__LFDataCache__
#define __CHAOSFramework__LFDataCache__

#include <vector>

#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/detail/atomic_count.hpp>


#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/SlbCachedInfo.h>
#include <chaos/common/data/cache/ChannelValueAccessor.h>

namespace chaos {
    namespace common {
		namespace data {
			
			namespace cache {
				
                //forward declaration
				class KeyGroupCache;
				
				
                //!Channel cache
				/*!
				 This class will manage the single channel cache. It work using a two position array
				 that has the write and read ptr to the SlbCachedInfo. When new value is wrote, the old
				 ReadPTR is remove and the index is swapped. this permit to promote the WritePtr as new Read Ptr
				 and a new slab is create for permit the new value storage.
				 */
				class LFDataCache {
                    //!permit the KeyGroupCache class to use the private variable
					friend class KeyGroupCache;
					
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
					uint32_t maxLength;
					
                    //! slab size
					uint32_t slabRequiredSize;
					
                    //!slab id to use
					uint16_t slabID;
					
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
					LFDataCache(memory::ManagedMemory *_memoryPool);
					
                    //! Default destructor
					~LFDataCache();
					
					/*!
					 Delete the unused cached slab
					 */
					void garbageCache();
					
					//! Initialize the channel cache
					/*!
					 Inizialize all the structure necassary to cache the serialized fragment for the
					 channel value
					 */
					void init(const char *name, uint32_t maxLength = 0);
					
					//! Set (and cache) the new value of the channel
					/*!
					 Set the new value of the channel executing the operation descripted below:
					 - generate or update the serialized fragment for the current channel value,
					 into the writeable slab pointer;
					 - swap the writeable with the readeable pointer using the index array
					 */
					void updateValue(const void* value, uint32_t legth = 0);
					
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
}

#endif /* defined(__CHAOSFramework__LFDataCache__) */
