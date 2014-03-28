/*
 *	vfs.h
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

#ifndef CHAOSFramework_Header_h
#define CHAOSFramework_Header_h

#include <string>
#include <stdint.h>

namespace chaos {
	namespace data_service {
	
		//!Namesapace for grupping all api and struct for the !CHAOS virtual filesystem
		namespace vfs {
			//forward declaration
			class VFSDriver;
			
			/*!
			 define the different type of block of data
			 withing !CHAOS VFS
			 */
			namespace block_type {
			typedef enum BlockType {
				BlockTypeStage,
				BlockTypeData
			} BlockType;
			}
			
			/*!
			 define the different type of block of data
			 withing !CHAOS VFS
			 */
			namespace block_flag {
			typedef enum BlockFlag {
				BlockFlagNone		= 0,
				BlockFlagWriteble	= 1,
				BlockFlagReadeable	= 2,
			} BlockFlag;
			}
			
			//! define a block of data into !CHAOS VFS
			struct DataBlock {
				//! the type of the block
				block_type::BlockType	type;
				
				//! the flag for this bloc
				unsigned int flags;
				
				//! is a precalculated timestamp after which the
				//! block became read only
				uint64_t	invalidation_timestamp;
				
				//! the precalculated max reacheable size for the block
				uint64_t	max_reacheable_size;
				
				//! block path
				std::string path;
				
				//! point to implementation usufull data
				void *opaque_ref;
			};
			
		}
	}
}

#endif
