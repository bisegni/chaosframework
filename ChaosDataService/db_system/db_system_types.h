/*
 *	index_systsem_types.h
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
#ifndef CHAOSFramework_index_system_types_h
#define CHAOSFramework_index_system_types_h

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

#include "../vfs/DataBlock.h"

namespace chaos {
	namespace data_service {
		namespace db_system {
			
			//! definition of the datapack index
			/*!
			 Structure for the managment of the datapack index
			 */
			struct DataPackIndex {
				//! unique identification of the instrumentation that has produced the data
				std::string did;
				
				//! the timestamp that identifying the time qhen the data pack has been collected
				uint64_t	acquisition_ts;
				
				//! location whithin the destination file
				chaos::data_service::vfs::FileLocationPointer dst_location;
				
				//! size of the pack
				uint32_t	datapack_size;
			};
			
			//! data pack locato returned by the cursor
			struct DataPackIndexQueryResult {
				//location of the found data pack
				chaos::data_service::vfs::PathFileLocation *dst_location;
				
				//! size of the pack
				uint32_t	datapack_size;
				
				DataPackIndexQueryResult():dst_location(NULL), datapack_size(0){};
				~DataPackIndexQueryResult(){if(dst_location) delete dst_location;}
			};
			
			//! define the query that can be applied to the DataPackINdex
			struct DataPackIndexQuery {
				//! start timestamp
				uint64_t	start_ts;
				
				//! end timestamp
				uint64_t	end_ts;
				
				//! device identification
				std::string did;
			};
			
			//! db server list type
			typedef std::vector<std::string>			DBDriverServerList;
			typedef std::vector<std::string>::iterator	DBDriverServerListIterator;
			
			//! index driver setting
			typedef struct DBDriverSetting {
				//! vector for all index endpoint url (usualy database url, more is the driver permit the scalability of the service)
				DBDriverServerList servers;
				
				//! kv param for the implementations of the driver
				std::map<std::string, std::string> key_value_custom_param;
			} DBDriverSetting;
		}
	}
}

#endif
