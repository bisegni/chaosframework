//
//  DatasetCache.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/21/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DatasetCache__
#define __CHAOSFramework__DatasetCache__

#include <map>
#include <string>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/ChannelCache.h>
#include <chaos/common/bson/util/builder.h>
#include <chaos/common/utility/ISDInterface.h>
#include <chaos/common/cconstants.h>

namespace chaos {
    
    namespace data {
        
        namespace cache {
            
                //! summary infromatio about channel
            typedef struct ChannelInfo {
                uint                        index;
                uint32_t                    dimension;
                chaos::DataType::DataType   type;
            } ChannelInfo;
            
            //!Channel oriented cache
            /*!
             This cache permit to store data oriented to channel (input or output).
             A channel is identified by a name, at the initizialization phase the
             all channel are organized by position and all value are se to null.
             */
            class DatasetCache: protected utility::ISDInterface  {
                    //! slab managed memory pool
                memory::ManagedMemory memoryPool;
                
                    //! the minimum size of the channel
                uint32_t minimumChannelSize;
                
                    //! the maximum size of the channel
                uint32_t maximumChannelSize;
                
                    //! bson generator
                bson::BufBuilder bsonBuilder;
                
                    //!Channel cache Array
                /*!
                 This array is create after all channel are be setuped before the init method call
                 */
                ChannelCache **chCachePtrArray;
            protected:
                //! this map associate the name of the channel to it's position
                std::map<std::string, ChannelInfo*> channelInfoMap;
                
            public:
                DatasetCache();
                virtual ~DatasetCache();
                //! initializatin of the channel cache
                /*!
                 Initialize the channel caching infrastructure
                 \param initParam is the pointer to a ChannelCacheSettings
                 structure
                 */
                void init(void* initParam) throw(chaos::CException);
                
                //! Start the implementation
                void start() throw(chaos::CException);
                
                //! Start the implementation
                void stop() throw(chaos::CException);
                
                //! Deinit the implementation
                void deinit() throw(chaos::CException);
                
                void garbageCache();
                
                //!add a new channel
                /*!
                 Add a new channel, organize all it's need to be cached,
                 it's element is created in cache and is set to null. it is important this
                 mehtod need to be called before init method
                 \param channelName is the string representation of the channel
                 \param the type of the channel
                 \param channelDimension is the channel dimension in case his type is note a base type
                 \return error status (0 = NOERR)
                 */
                int addChannel(const char *channelName,  chaos::DataType::DataType type, uint32_t channelDimension = 0);
                
                void updateChannelValue(const char *channelName, const void* channelValue, uint32_t valueLegth = 0);
                
                void updateChannelValue(uint16_t channelIndex, const void* channelValue, uint32_t valueLegth = 0);
                
                SlbCachedInfoPtr getCurrentChannelCachedValue(const char *channelName);
                
                SlbCachedInfoPtr getCurrentChannelCachedValue(uint16_t channelIndex);
            };
            
        }
    }
    
}

#endif /* defined(__CHAOSFramework__DatasetCache__) */
