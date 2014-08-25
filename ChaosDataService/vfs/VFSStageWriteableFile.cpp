/*
 *	VFSStageWriteableFile.cpp
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

#include "VFSStageWriteableFile.h"
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::data_service::vfs;

#define VFSStageWriteableFile_LOG_HEAD "[VFSStageWriteableFile] - " << getVFSFileInfo()->vfs_fpath << " - "
#define VFSWF_LAPP_ LAPP_ << VFSStageWriteableFile_LOG_HEAD
#define VFSWF_LDBG_ LDBG_ << VFSStageWriteableFile_LOG_HEAD << __FUNCTION__ << " - "
#define VFSWF_LERR_ LERR_ << VFSStageWriteableFile_LOG_HEAD << __FUNCTION__ << " - "


VFSStageWriteableFile::VFSStageWriteableFile(storage_system::StorageDriver *_storage_driver_ptr,
											 index_system::IndexDriver *_index_driver_ptr,
											 std::string stage_vfs_relative_path):
VFSStageFile(_storage_driver_ptr,
			 _index_driver_ptr,
			 stage_vfs_relative_path,
			 VFSStageFileOpenModeWrite) {
	
}


//! check if datablock is valid according to internal logic
bool VFSStageWriteableFile::isDataBlockValid(DataBlock *new_data_block_ptr) {
	check_validity_counter++;
	if(!new_data_block_ptr) return false;
	
	if((check_validity_counter % 16)) return true;
	
	//check standard check
	return VFSFile::isDataBlockValid(new_data_block_ptr);
}

// write data on the current data block
int VFSStageWriteableFile::write(void *data, uint32_t data_len) {
	int err = 0;
	if(!isDataBlockValid(current_data_block)) {
		if((err = VFSFile::releaseDataBlock(current_data_block))) {
			VFSWF_LERR_ << "Error releaseing datablock " << err;
		} else if((err = VFSFile::getNewDataBlock(&current_data_block))) {
			VFSWF_LERR_ << "Error creating datablock " << err;
		}
	}
	
	if( err || (err = VFSStageFile::write(data, data_len))) {
		VFSWF_LERR_ << "Error writing to datablock " << err;
	}
	return err ;
}