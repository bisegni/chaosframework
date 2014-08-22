/*
 *	VFSDataWriteableFile.cpp
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

#include "VFSDataWriteableFile.h"
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::data_service::vfs;

#define VFSDataWriteableFile_LOG_HEAD "[VFSDataWriteableFile] - " << getVFSFileInfo()->vfs_fpath << " - "
#define VFSWF_LAPP_ LAPP_ << VFSDataWriteableFile_LOG_HEAD
#define VFSWF_LDBG_ LDBG_ << VFSDataWriteableFile_LOG_HEAD
#define VFSWF_LERR_ LERR_ << VFSDataWriteableFile_LOG_HEAD <<__LINE__<< " - "


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VFSDataWriteableFile::VFSDataWriteableFile(storage_system::StorageDriver *_storage_driver_ptr,
										   index_system::IndexDriver *_index_driver_ptr,
										   std::string data_vfs_relative_path):
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VFSDataFile(_storage_driver_ptr,
			_index_driver_ptr,
			data_vfs_relative_path,
			VFSDataFileOpenModeWrite) {
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool VFSDataWriteableFile::isDataBlockValid(DataBlock *new_data_block_ptr) {
	//check operational value
	return VFSFile::isDataBlockValid(new_data_block_ptr);

}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::switchDataBlock() {
	int err = 0;
	if((err = VFSFile::releaseDataBlock(current_data_block))) {
		VFSWF_LERR_ << "Error releaseing datablock " << err;
	} else if((err = VFSFile::getNewDataBlock(&current_data_block))) {
		VFSWF_LERR_ << "Error creating datablock " << err;
	}
	return err;
}
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int64_t VFSDataWriteableFile::getCurrentOffSet() {
	return current_data_block == NULL ?-1:current_data_block->current_size;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::write(void *data, uint32_t data_len) {
	int err = 0;
	if(!isDataBlockValid(current_data_block)) {
		if((err = VFSFile::releaseDataBlock(current_data_block))) {
			VFSWF_LERR_ << "Error releaseing datablock " << err;
		}
		
		if((err = VFSFile::getNewDataBlock(&current_data_block))) {
			VFSWF_LERR_ << "Error creating datablock " << err;
		}
	}
	return VFSFile::write(data, data_len);
}