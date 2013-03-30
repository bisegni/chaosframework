/*
 *	DataCache.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
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

#ifndef __CHAOSFramework__DataCache__
#define __CHAOSFramework__DataCache__

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/cache/memcached.h>
#include <chaos/common/utility/ISDInterface.h>
namespace chaos {
    namespace datacache {
        
        //! Structure for the cache settings
        typedef struct CacheInitParameter {
            int chunkSize;
            double factor;
            int maxItemByte;
            size_t maxByte;
        } CacheInitParameter;
        
        //! CHoas data caching subsistem
        /*!
         Data cache is the central caching sytem. Is is a singleton and only one instance can be active
         in one process
         */
        class DataCache : public Singleton<DataCache>{
            friend class Singleton<DataCache>;
            
        protected:
            
            //! Constructor Method
            /*!
             */
            DataCache();
            
            
            //! Destructor method
            /*!
             */
            ~DataCache();
        
        public:
            
            //! Initialize instance
            void init(void* initParam) throw(chaos::CException);
            
            //! Start the implementation
            void start() throw(chaos::CException);
            
            //! Start the implementation
            void stop() throw(chaos::CException);
            
            //! Deinit the implementation
            void deinit() throw(chaos::CException);
            
            //! Associate a data buffer to a key
            int storeItem(const char *key, const void *buffer, int32_t bufferLen);
            
            //! Return the item asscoiated at the key
            int getItem(const char *key, int32_t& nkey, void **returnBuffer);
            
            //! delete the caching associate at the key
            int deleteItem(const char *key);
        };
    }
}

#endif /* defined(__CHAOSFramework__DataCache__) */
