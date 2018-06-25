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

#ifndef __CHAOSFramework__CacheDriver__
#define __CHAOSFramework__CacheDriver__

//#include "../dataservice_global.h"

#include <stdint.h>
#include <string>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/Buffer.hpp>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

#include "cache_system_types.h"

namespace chaos {
    namespace data_service {
        namespace cache_system {
            
            typedef chaos::common::data::BufferSPtr CacheData;
            
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
									CacheData data_to_store) = 0;
                
                virtual int getData(const std::string& key,
									CacheData& cached_data) = 0;
                
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
