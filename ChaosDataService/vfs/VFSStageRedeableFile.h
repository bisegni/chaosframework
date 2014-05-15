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
			class VFSStageRedeableFile : public VFSStageFile {
				friend class VFSManager;
				
				//!keep track of the totale byte read for the local block
				uint64_t global_readed_byte;
				
				//! Default constructor
				VFSStageRedeableFile(chaos_data_storage::StorageDriver *_storage_driver_ptr, chaos_data_index::IndexDriver *_index_driver_ptr, std::string stage_vfs_relative_path);
				
				//! Get next availbale block
				/*!
				 Next block is loaded from the set of block
				 that are in state DataBlockState::DataBlockStateNone
				 */
				inline void getNextAvailbaleBlock();
			public:
				// write data on the current data block
				int read(void *data, uint32_t data_len);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSStageRedeableFile__) */
