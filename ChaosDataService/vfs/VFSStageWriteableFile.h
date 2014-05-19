/*
 *	VFSStageWriteableFile.h
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

#ifndef __CHAOSFramework__VFSStageWriteableFile__
#define __CHAOSFramework__VFSStageWriteableFile__

#include "VFSStageFile.h"

namespace chaos {
	namespace data_service {
		namespace vfs {
			//forward declaration
			class VFSManager;
			
			//! Stage writeable file
			/*!
			 Manage the write operation on the stage file
			 */
			class VFSStageWriteableFile: public VFSStageFile {
				friend class VFSManager;
				
				//counter for release the stress on the timining check
				uint32_t check_validity_counter;
				
				//! check if datablock is valid according to internal logic
				inline bool isDataBlockValid(DataBlock *new_data_blok_handler);

				VFSStageWriteableFile(storage_system::StorageDriver *_storage_driver_ptr, index_system::IndexDriver *_index_driver_ptr, std::string stage_vfs_relative_path);

			public:
				// write data on the current data block
				int write(void *data, uint32_t data_len);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSStageWriteableFile__) */
