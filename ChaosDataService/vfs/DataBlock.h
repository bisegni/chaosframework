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
		
		namespace db_system {
			class DBDriver;
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
				
				DataBlock():flags(0),
							invalidation_timestamp(0),
							max_reacheable_size(0),
							current_work_position(0),
							creation_time(0),
							driver_private_data(NULL){}
			};
			
			class PathFileLocation {
			protected:
				//! location into the virtual file of the datablock
				std::string block_vfs_path;
				//! is th offset, whitin the datablock, where the datapack begin
				uint64_t offset;
				
			public:
				PathFileLocation():block_vfs_path(""), offset(0){}
				PathFileLocation(const std::string& _block_vfs_path,
								 uint64_t _offset):
				block_vfs_path(_block_vfs_path),
				offset(_offset){};
				
				~PathFileLocation(){};
				
				virtual bool isValid() {return (offset != 0)?block_vfs_path.size()>0:true;}
				
				virtual uint64_t getOffset(){return offset;}
				
				virtual std::string getBlockVFSPath() {return block_vfs_path;}
			};
			
			//! define a pointer to a location whitin a virtual file
			class FileLocationPointer :
				public PathFileLocation {
				friend class chaos::data_service::db_system::DBDriver;
				friend class VFSFile;
				
				//! is the datablock of the destination datafile where the data pack is definitely stored
				DataBlock *data_block;
			
			public:
				FileLocationPointer():PathFileLocation(), data_block(NULL){}
				FileLocationPointer(DataBlock *_data_block, uint64_t _offset):PathFileLocation(), data_block(_data_block){
					if(data_block) block_vfs_path = _data_block->vfs_path;
					offset = _offset;
				}
				~FileLocationPointer(){};
				
				bool isValid() {
					return (offset != 0)?(data_block != NULL):PathFileLocation::isValid();
				}
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__DataBlock__) */
