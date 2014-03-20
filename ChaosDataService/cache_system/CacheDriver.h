/*
 *	CacheDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__CacheDriver__
#define __CHAOSFramework__CacheDriver__

#include "../dataservice_global.h"

#include <stdint.h>
#include <string>

#include <chaos/common/utility/NamedService.h>
namespace chaos {
    namespace data_service {
        namespace cache_system {
            
                //! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the 
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
            class CacheDriver : public chaos::NamedService {
			protected:
				CacheDriver(std::string alias);
            public:
				virtual ~CacheDriver();
				
                virtual int putData(void *element_key, uint8_t element_key_len, void *value, uint32_t value_len) = 0;
                
                virtual int getData(void *element_key, uint8_t element_key_len, void **value, uint32_t& value_len) = 0;

                virtual int addServer(std::string server_desc) = 0;
                
                virtual int removeServer(std::string server_desc) = 0;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriver__) */
