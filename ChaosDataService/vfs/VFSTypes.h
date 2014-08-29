/*
 *	VFSTypes.h
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


#ifndef CHAOSFramework_VFSTypes_h
#define CHAOSFramework_VFSTypes_h

#include <string>
#include <stdint.h>
#include "VFSConsts.h"

namespace chaos {
	namespace data_service {
		namespace vfs {
			
			//! describe the local domain for the VFS
			typedef struct VFSDomain {
				std::string name;
				std::string unique_code;
				std::string local_url;
			} VFSDomain;
			
			namespace block_seek_base {
				typedef enum BlockSeekBase {
					BlockSeekBaseBegin		= 0,
					BlockSeekBaseCurrent	= 1,
					BlockSeekBaseEnd		= 2
				} BlockSeekBase;
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
			
			namespace data_block_state {
				//! define a type of work that can occour on stage datablock
				typedef enum DataBlockState {
					//! block is not used for processing or acquisition
					DataBlockStateNone			= 0,
					
					//! block is acquiring new data
					DataBlockStateAquiringData	= 1,
					
					//! block is reading for processing
					DataBlockStateProcessing	= 2,
					
					//! block as been processed
					DataBlockStateProcessed		= 4,
					
					//! block as been locked for processing
					DataBlockStateLocked		= 8,
					
					//! block has been unlocked
					DataBlockStateUnlocked		= 16,
					
					//! can be querable
					DataBlockStateQuerable		= 32
				} DataBlockState;
			}
			
			typedef struct VFSFileInfo {
				uint32_t		max_block_size;
				uint32_t		max_block_lifetime;
				std::string		identify_key;
				std::string		vfs_fpath;
				std::string		vfs_domain;
			} VFSFileInfo;

		}
	}
}

#endif
