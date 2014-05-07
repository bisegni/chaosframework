//
//  VFSTypes.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_VFSTypes_h
#define CHAOSFramework_VFSTypes_h

#include <string>
#include <stdint.h>

namespace chaos {
	namespace data_service {
		namespace vfs {
			
			//! describe the local domain for the VFS
			typedef struct VFSDomain {
				std::string name;
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
					DataBlockStateUnlocked		= 12
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
