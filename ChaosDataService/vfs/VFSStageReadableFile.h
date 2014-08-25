/*
 *	VFSStageRedeableFile.h
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
#ifndef __CHAOSFramework__VFSStageRedeableFile__
#define __CHAOSFramework__VFSStageRedeableFile__

#include "VFSStageFile.h"

namespace chaos {
	namespace data_service {
		namespace vfs {
			
			//forward declaration
			class VFSManager;
			
			//! Stage readable file
			/*!
			 Manage the read operation on the stage file
			 */
			class VFSStageReadableFile : public VFSStageFile {
				friend class VFSManager;
				//param overlaped_block_read if tru the read operation will fullfill the buffer
				//if there are other available data block
				bool overlaped_block_read;
				uint64_t current_block_creation_ts;
				
				//! Default constructor
				VFSStageReadableFile(storage_system::StorageDriver *_storage_driver_ptr, index_system::IndexDriver *_index_driver_ptr, std::string stage_vfs_relative_path);
				
				//! Get next availbale block
				/*!
				 Next block is loaded from the set of block
				 that are in state DataBlockState::DataBlockStateNone
				 */
				inline int getNextAvailbaleBlock();
				
				inline int checkForBlockChange(bool overlapping);
			public:
				// write data on the current data block
				/*!
				 the read function try to fill all the buffer, if the block is full readed
				 and other space if left in the buffer, new block is loaded according to
				 overlaped_block_read parameter
				 \param data buffer where put readed byte
				 \param data_len the length of the buffer
				 */
				int read(void *data, uint32_t data_len);
				
				int syncJurnal();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSStageRedeableFile__) */
