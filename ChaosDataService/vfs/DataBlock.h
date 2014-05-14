/*
 *	DataBlock.h
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

#ifndef __CHAOSFramework__DataBlock__
#define __CHAOSFramework__DataBlock__
#include <stdint.h>
#include <string>

#include "VFSTypes.h"

namespace chaos {
	namespace data_service {
		
		namespace storage_system {
			class StorageDriver;
		}
		
		namespace index_system {
			class IndexDriver;
		}
		
		//!Namesapace for grupping all api and struct for the !CHAOS virtual filesystem
		namespace vfs {
			//forward declaration
			class VFSDriver;

			//! define a block of data into !CHAOS VFS
			struct DataBlock {
				
				//! the flag for this bloc
				unsigned int flags;
				
				//! is a precalculated timestamp after which the
				//! block became read only
				uint64_t	invalidation_timestamp;
				
				//! the precalculated max reacheable size for the block
				uint64_t	max_reacheable_size;
				
				//!current datablock size
				uint64_t	current_size;
				
				//!current datablock size
				uint64_t	creation_time;
				
				//! block path relative to the domain,
				//! it not consider the root path of driver configuration
				char *vfs_path;
				
				//! driver private data
				void *driver_private_data;
				
			private:
				friend class chaos::data_service::storage_system::StorageDriver;
				friend class chaos::data_service::index_system::IndexDriver;
				//! privade constructor
				DataBlock();
				//! private destructor
				virtual ~DataBlock();
				
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__DataBlock__) */
