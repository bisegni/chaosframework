//
//  Dataset.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/21/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ChannelCache__
#define __CHAOSFramework__ChannelCache__

#include <map>
#include <string>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/bson/util/builder.h>

namespace chaos {
    
    namespace data {
        
        namespace cache {
            
            //!Channel oriented cache
            /*!
             This cache permit to store data oriented to channel (input or output).
             A channel is identified by a name, at the initizialization phase the
             all channel are organized by position and all value are se to null.
             */
            class Dataset : protected DataCache {
                //! the minimum size of the channel
                uint32_t minimumChannelSize;
                
                //! the maximum size of the channel
                uint32_t maximumChannelSize;
                
                //! bson generator
                bson::BufBuilder bsonBuilder;
            protected:
                //! this map associate the name of the channel to it's position
                std::map<std::string, int> channelPostionMap;
                
            public:
                
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
                
                //!add a new channel
                /*!
                 Add a new channel, organize all it's need to be cached,
                 it's element is created in cache and is set to null. it is important this
                 mehtod need to be called before init method
                 \param channelName is the string representation of the channel
                 \param channelDimension is the channel dimension
                 \return error status (0 = NOERR)
                 */
                int addChannel(const char *channelName, uint32_t channelDimension);
                
                
                //! delete item
                int deleteItem(const char *key);
            };
            
        }
    }
    
}

#endif /* defined(__CHAOSFramework__ChannelCache__) */
