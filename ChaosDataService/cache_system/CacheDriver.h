//
//  CacheDriver.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 15/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__CacheDriver__
#define __CHAOSFramework__CacheDriver__

namespace chaos {
    namespace data_service {
        namespace cache_system {
            
                //! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the 
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
            class CacheDriver {
            public:
                virtual int putData(const char *key, void *value, void *value_len) = 0;
                
                virtual int getData(const char *key, void *value, void *value_len) = 0;
                
                virtual int addServer(std::string server_desc) = 0;
                
                virtual int removeServer(std::string server_desc) = 0;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriver__) */
