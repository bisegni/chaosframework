/*
 *	VFSStageReadableFile.cpp
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
#include "VFSStageReadableFile.h"

using namespace chaos::data_service::vfs;

#define VFSStageReadableFile_LOG_HEAD "[VFSStageReadableFile] - " << getVFSFileInfo()->vfs_fpath << " - "
#define VFSRF_LAPP_ LAPP_ << VFSStageReadableFile_LOG_HEAD
#define VFSRF_LDBG_ LDBG_ << VFSStageReadableFile_LOG_HEAD << __FUNCTION__ << " - "
#define VFSRF_LERR_ LERR_ << VFSStageReadableFile_LOG_HEAD << __FUNCTION__ << " - "


VFSStageReadableFile::VFSStageReadableFile(storage_system::StorageDriver *_storage_driver_ptr,
										   index_system::IndexDriver *_index_driver_ptr,
										   std::string stage_vfs_relative_path):
VFSStageFile(_storage_driver_ptr,
			 _index_driver_ptr,
			 stage_vfs_relative_path,
			 VFSStageFileOpenModeWrite),
current_block_creation_ts(0) {
}

int VFSStageReadableFile::getNextAvailbaleBlock() {
	int err = 0;
	bool success = false;
	
	//get next available datablock
	while (!current_data_block && !err) {
		err = index_driver_ptr->vfsFindSinceTimeDataBlock(this,
														  current_block_creation_ts,
														  true,
														  vfs::data_block_state::DataBlockStateNone,
														  &current_data_block);
		if(err || !current_data_block)	break; //break on error
		
		//try to update the current state to processing
		err = index_driver_ptr->vfsSetStateOnDataBlock(this,
													   current_data_block,
													   vfs::data_block_state::DataBlockStateNone,
													   vfs::data_block_state::DataBlockStateProcessing,
													   success);
		if((err || !success) ||
		   (success && (err = storage_driver_ptr->openBlock(current_data_block, block_flag::BlockFlagReadeable)))) {
			//the state has not been changed for some reason
			//find another owne
			current_block_creation_ts = 0;
			delete(current_data_block);
			current_data_block = NULL;
		} else {
			current_block_creation_ts = current_data_block->creation_time;
		}
	}
	
	//change the state
	return err;
}

int VFSStageReadableFile::checkForBlockChange(bool overlapping) {
	int err = 0;
	uint64_t  cur_position = 0;
	
	if(!current_data_block) {
		if((err = getNextAvailbaleBlock()))  return err;
	} else {
		if(storage_driver_ptr->tell(current_data_block, cur_position)){
			return err;
		} else {
			if(cur_position == current_data_block->max_reacheable_size) {
				//set current datablock to processed state
				if((err = updateDataBlockState(data_block_state::DataBlockStateProcessed))) {
					return err;
				}
				
				//close the data block
				err = storage_driver_ptr->closeBlock(current_data_block);
				current_data_block = NULL;
				if(err) {
					VFSRF_LERR_ << "Error closing datablock";
					return err;
				}
				
				if(overlapping) {
					//we need to load another block, in this we have read all
					if((err = getNextAvailbaleBlock()))  return err;
				}
			}
		}
	}
	return 0;
}

// write data on the current data block
int VFSStageReadableFile::read(void *data, uint32_t data_len, bool overlaped_block_read) {
	int       err = 0;
	int       read_data = 0;
	
	
	//check if we need to change block
	if((err = checkForBlockChange(overlaped_block_read)) || !current_data_block) return err;
	
	//read data from block
	read_data = VFSFile::read(data, data_len);
	
	if(overlaped_block_read &&
	   read_data < data_len) {
		if( (err = checkForBlockChange(overlaped_block_read))) return err;
		if(!current_data_block) return read_data;
		//try to load the
		read_data = VFSFile::read((void*)((char*)data+read_data), data_len - read_data);
	}
	return read_data;
}