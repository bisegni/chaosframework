/*
 *	VFSStageRedeableFile.cpp
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
#include "VFSStageRedeableFile.h"

using namespace chaos::data_service::vfs;

#define VFSStageRedeableFile_LOG_HEAD "[VFSStageRedeableFile] - " << getVFSFileInfo()->vfs_fpath << " - "
#define VFSRF_LAPP_ LAPP_ << VFSStageRedeableFile_LOG_HEAD
#define VFSRF_LDBG_ LDBG_ << VFSStageRedeableFile_LOG_HEAD << __FUNCTION__ << " - "
#define VFSRF_LERR_ LERR_ << VFSStageRedeableFile_LOG_HEAD << __FUNCTION__ << " - "


VFSStageRedeableFile::VFSStageRedeableFile(chaos_data_storage::StorageDriver *_storage_driver_ptr,
										   chaos_data_index::IndexDriver *_index_driver_ptr,
										   std::string stage_vfs_relative_path):
VFSStageFile(_storage_driver_ptr,
			 _index_driver_ptr,
			 stage_vfs_relative_path,
			 VFSStageFileOpenModeWrite),
global_readed_byte(0) {
	
}

void VFSStageRedeableFile::getNextAvailbaleBlock() {
	
}

// write data on the current data block
int VFSStageRedeableFile::read(void *data, uint32_t data_len) {
	if(!current_data_block) 
	return 0;
}