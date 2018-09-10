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

#ifndef __CHAOSFramework__KeyGroupCache__
#define __CHAOSFramework__KeyGroupCache__

#include <map>
#include <string>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/data/cache/LFDataCache.h>
#include <chaos/common/data/cache/ChannelValueAccessor.h>

namespace chaos {
    namespace common {
		namespace data {
			
			namespace cache {
				
                //! summary infromatio about channel
				typedef struct ChannelInfo {
					uint8_t                     index;
					uint32_t                    maxLength;
					chaos::DataType::DataType   type;
				} ChannelInfo;
				
				//!Channel oriented cache
				/*!
				 This cache permit to store data oriented to channel (input or output).
				 A channel is identified by a name, at the initizialization phase the
				 all channel are organized by position and all value are se to null.
				 */
				class KeyGroupCache: protected utility::StartableService  {
                    //! slab managed memory pool
					memory::ManagedMemory memoryPool;
					
                    //! the minimum size of the channel
					uint32_t minimumChannelSize;
					
                    //! the maximum size of the channel
					uint32_t maximumChannelSize;
					
                    //!Channel cache Array
					/*!
					 This array is create after all channel are be setuped before the init method call
					 */
					LFDataCache **chCachePtrArray;
				protected:
					//! this map associate the name of the channel to it's position
					std::map<std::string, ChannelInfo*> channelInfoMap;
					
				public:
					KeyGroupCache();
					virtual ~KeyGroupCache();
					//! initializatin of the channel cache
					/*!
					 Initialize the channel caching infrastructure
					 \param initParam is the pointer to a LFDataCacheSettings
					 structure
					 */
					void init(void* initParam);
					
					//! Start the implementation
					void start();
					
					//! Start the implementation
					void stop();
					
					//! Deinit the implementation
					void deinit();
					
					void garbageCache();
					
					//!add a new channel
					/*!
					 Add a new channel, organize all it's need to be cached,
					 it's element is created in cache and is set to null. it is important this
					 mehtod need to be called before init method
					 \param channelName is the string representation of the channel
					 \param the type of the channel
					 \param channelMaxLength is the channel maximum length in case his type is note a base type
					 \return error status (0 = NOERR)
					 */
					int addKeyInfo(const char *key,  chaos::DataType::DataType type, uint32_t channelMaxLength = 0);
					
					void updateKeyValue(const char *key, const void* channelValue, uint32_t valueLegth = 0);
					
					void updateKeyValue(uint16_t keyIndex, const void* channelValue, uint32_t valueLegth = 0);
					
					SlbCachedInfoPtr getCurrentKeyValue(const char *key);
					
					SlbCachedInfoPtr getCurrentKeyValue(uint16_t keyIndex);
					
					void getCurrentKeyAccessor(const char *key, ChannelValueAccessor& accessorPtr);
					
					void getCurrentKeyAccessor(uint16_t keyIndex, ChannelValueAccessor& accessorPtr);
				};
				
			}
		}
    }
    
}

#endif /* defined(__CHAOSFramework__KeyGroupCache__) */
