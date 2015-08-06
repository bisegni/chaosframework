/*
 *	DataBlockCache.h
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

#ifndef __CHAOSFramework__DataBlockCache__
#define __CHAOSFramework__DataBlockCache__

#include <map>
#include <string>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/async_central/async_central.h>

#include <boost/thread.hpp>

#include "DataBlockFetcher.h"
#include "../storage_system/StorageDriver.h"

namespace chaos {
	namespace data_service {
		namespace vfs {
			class VFSManager;
			
			namespace query {
				
				class DataBlockFetcher;
				
				//! Caching of the datablock usage
				class DataBlockCache:
				public common::utility::Singleton<DataBlockCache>,
				public common::utility::InizializableService,
				public chaos::common::async_central::TimerHandler {
					friend class Singleton<DataBlockCache>;
					friend class chaos::data_service::vfs::VFSManager;
					
					storage_system::StorageDriver *storage_driver;
					
					//! the map that collect the datablock for path
					//! mutext on datablock map
					boost::shared_mutex map_path_datablock_mutex;
					typedef std::map<std::string, query::DataBlockFetcher* >::iterator MapPathDatablockIterator;
					std::map<std::string, query::DataBlockFetcher* > map_path_datablock;
					
					// TimerHandler callback
					void timeout();
					
					//clean all or unused data block fetcher
					void clean(bool all_unused);
										
				protected:
					//! Initialize instance
					void init(void *init_data) throw(chaos::CException);
					
					//! Deinit the implementation
					void deinit() throw(chaos::CException);
					
				public:
					//! Get a new fetcher for datablock
					int getFetcherForBlockWithPath(const std::string& datablock_path,
												   DataBlockFetcher **fetcher);
					
					//! Release a previously allocated fetcher for datablock
					int releaseFetcher(DataBlockFetcher *datablock_fetcher);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DataBlockCache__) */
