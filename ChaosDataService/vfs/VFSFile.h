/*
 *	VFSFile.h
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
#ifndef __CHAOSFramework__VFSFile__
#define __CHAOSFramework__VFSFile__

#include "VFSTypes.h"
#include "../index_system/IndexDriver.h"
#include "storage_system/StorageDriver.h"


namespace chaos {
	namespace data_service {
		namespace vfs {
		
			namespace chaos_index = chaos::data_service::index_system;
			
			class VFSManager;
			struct DataBlock;
			
						
			//! VFS Logical file
			class VFSFile {
				friend class VFSManager;
				
				//! operational setting for the virtual file
				VFSFileInfo vfs_file_info;
				
			protected:
				//!tag if the file is in good state
				bool good;

				//! Current available data block where read or write
				DataBlock *current_data_block;
				
				//!index driver pointer
				chaos_index::IndexDriver *index_driver_ptr;
				
				//!storage driver pointer
				storage_system::StorageDriver *storage_driver_ptr;
			
				//! return new datablock where write into
				int getNewDataBlock(DataBlock **new_data_block_handler);
				
				//! return new datablock where write into
				int getNextInTimeDataBlock(DataBlock **new_data_block_handler, uint64_t timestamp, data_block_state::DataBlockState state);
				
				//! change Datablock state
				int updateDataBlockState(data_block_state::DataBlockState state);
				
				//! release a datablock
				int releaseDataBlock(DataBlock *data_block_ptr);
				
				//default consturctor or destructor
				VFSFile(storage_system::StorageDriver *_storage_driver_ptr, index_system::IndexDriver *_index_driver_ptr, std::string area, std::string vfs_fpath);
				~VFSFile();
			public:
				//! Get the VFS information for file
				const VFSFileInfo *getVFSFileInfo() const;
				
				//! Check if the file exists
				bool exist();
				
				//! Return the goodness of the file
				bool isGood();
				
				virtual int seekOnCurrentBlock(block_seek_base::BlockSeekBase base_direction, int64_t offset);
				
				//! write data on the current data block
				virtual int write(void *data, uint32_t data_len);
				
				//! read data from the current data block
				/*!
				 Read the data for the current loaded databloc
				 \param data pointer where put the readed byte
				 \param byte to read
				 \return negative number is something is not gone well,
				 otherwise the number of byte readed
				 */
				virtual int read(void *buffer, uint32_t buffer_len);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSFile__) */
