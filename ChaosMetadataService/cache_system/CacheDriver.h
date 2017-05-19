/*
 *	CacheDriver.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

//#include "../dataservice_global.h"

#include <stdint.h>
#include <string>

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

#include "cache_system_types.h"

namespace chaos {
    namespace data_service {
        namespace cache_system {
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(char, CacheData);
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, CacheData, MultiCacheData);
            
                //! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the 
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
            class CacheDriver :
			public common::utility::NamedService,
			public common::utility::InizializableService {
			protected:
				CacheDriverSetting *cache_settings;
				CacheDriver(std::string alias);
            public:
				virtual ~CacheDriver();
				
                virtual int putData(const std::string& key,
									const CacheData& data) = 0;
                
                virtual int getData(const std::string& key,
									CacheData& data) = 0;
                
                virtual int getData(const ChaosStringVector&    keys,
                                    MultiCacheData&             multi_data) = 0;

                virtual int addServer(std::string server_desc) = 0;
                
                virtual int removeServer(std::string server_desc) = 0;
				
				virtual int updateConfig() = 0;
				
				//! init
				/*!
				 Need a point to a structure DBDriverSetting for the setting
				 */
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriver__) */
