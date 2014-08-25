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
		
		namespace index_system {
			class IndexDriver;
		}
		
		//!Namesapace for grupping all api and struct for the !CHAOS virtual filesystem
		namespace vfs {

			class VFSFile;
			
			//! define a block of data into !CHAOS VFS
			struct DataBlock {
				
				//! the flag for this block
				unsigned int flags;
				
				//! is a precalculated timestamp after which the
				//! block became read only
				uint64_t	invalidation_timestamp;
				
				//! the precalculated max reacheable size for the block
				uint64_t	max_reacheable_size;
				
				//!current datablock position in file elaboration(write or read according to the state)
				uint64_t	current_work_position;
				
				//!current datablock size
				uint64_t	creation_time;
				
				//! block path relative to the domain,
				//! it not consider the root path of driver configuration
				std::string vfs_path;
				
				//! driver private data
				void *driver_private_data;
				
				//! data usefull for index driver where can put information to fastes find the datablock
				std::string index_driver_uinique_id;
				
				DataBlock():flags(0), invalidation_timestamp(0), max_reacheable_size(0), current_work_position(0), creation_time(0), driver_private_data(NULL){}
			};
			
			//! define a pointer to a location whitin a virtual file
			class FileLocationPointer {
				friend class chaos::data_service::index_system::IndexDriver;
				friend class VFSFile;
				
				//! is the datablock of the destination datafile where the data pack is definitely stored
				DataBlock *data_block;
				
				//! is th offset, whitin the datablock, where the datapack begin
				uint64_t offset;
				
			public:
				FileLocationPointer():data_block(NULL), offset(0){}
				FileLocationPointer(DataBlock *_data_block, uint64_t _offset):data_block(_data_block), offset(_offset){}
				~FileLocationPointer(){};
				
				bool isValid() {return (offset != 0)?(data_block != NULL):true;}
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__DataBlock__) */
