/*
 *	VFSFile.cpp
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

#include "VFSFile.h"

#include <string>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>

#define VFSFile_LOG_HEAD "[VFSFile] - " << vfs_file_info.vfs_fpath << " - "
#define VFSF_LAPP_ LAPP_ << VFSFile_LOG_HEAD
#define VFSF_LDBG_ LDBG_ << VFSFile_LOG_HEAD << __FUNCTION__ << " - "
#define VFSF_LERR_ LERR_ << VFSFile_LOG_HEAD << __FUNCTION__ << " - "


using namespace chaos::data_service::vfs;
namespace chaos_data_index = chaos::data_service::index_system;
namespace chaos_data_storage = chaos::data_service::storage_system;

VFSFile::VFSFile(chaos_data_storage::StorageDriver *_storage_driver_ptr, chaos_data_index::IndexDriver *_index_driver_ptr, std::string vfs_fpath) :
storage_driver_ptr(_storage_driver_ptr),
index_driver_ptr(_index_driver_ptr),
current_data_block(NULL) {
	vfs_file_info.vfs_fpath = vfs_fpath;
}

VFSFile::~VFSFile() {
	if(current_data_block) {
		releaseDataBlock(current_data_block);
		current_data_block = NULL;
	}
}

//! return new datablock where write into
int VFSFile::getNewDataBlock(DataBlock **new_data_block_handler) {
	DataBlock *new_data_block_ptr = NULL;
	std::string block_unique_path = boost::str(boost::format("%1%/%2%") % vfs_file_info.vfs_fpath % chaos::UUIDUtil::generateUUID());
	
	//open new block has writeable
	if(storage_driver_ptr->openBlock(block_unique_path, block_flag::BlockFlagWriteble, &new_data_block_ptr) || !new_data_block_ptr) {
		return -1;
	}

	//compute data block limits for this vfs configuraiton
	new_data_block_ptr->invalidation_timestamp = TimingUtil::getTimeStamp() + vfs_file_info.max_block_lifetime;
	new_data_block_ptr->max_reacheable_size = vfs_file_info.max_block_size;
	
	//ask to db wat is the next datablock
	if(index_driver_ptr->vfsAddNewDataBlock(this, new_data_block_ptr, data_block_state::DataBlockStateAquiringData)) {
		storage_driver_ptr->closeBlock(new_data_block_ptr);
		return -2;
	}
	*new_data_block_handler = new_data_block_ptr;
	DEBUG_CODE(VFSF_LDBG_ << "Opened datablock of path " << new_data_block_ptr->vfs_path);

	return 0;
}

//! return new datablock where write into
int VFSFile::getNextInTimeDataBlock(DataBlock **new_data_block_handler, uint64_t timestamp, data_block_state::DataBlockState state) {
	return 0;
}

//! change Datablock state
int updateDataBlockState(DataBlock *new_data_block_handler, data_block_state::DataBlockState state) {
	return 0;
}

//! release a datablock
int VFSFile::releaseDataBlock(DataBlock *data_block_ptr) {
	if(!data_block_ptr) return 0;
	int err = 0;
	DEBUG_CODE(VFSF_LDBG_ << "Release datablock of path " << data_block_ptr->vfs_path);
	//write on index for free of work block
	if((err = index_driver_ptr->vfsSetStateOnDataBlock(this, data_block_ptr, data_block_state::DataBlockStateNone))) {
		VFSF_LERR_ << "Error setting state on datablock with error " << err;
		return storage_driver_ptr->closeBlock(data_block_ptr);
	}
	//close the block
	return storage_driver_ptr->closeBlock(data_block_ptr);
}

const VFSFileInfo *VFSFile::getVFSFileInfo() const {
	return &vfs_file_info;
}

bool VFSFile::isGood() {
	return good;
}

bool VFSFile::exist() {
	int err = 0;
	bool exist_flag = false;
	if((err = index_driver_ptr->vfsFileExist(this, exist_flag))) {
		VFSF_LERR_ << "Error checking file existance with code " << err;
		return false;
	}
	return exist_flag;
}

int VFSFile::write(void *data, uint32_t data_len) {
	//write data
	return storage_driver_ptr->write(current_data_block, data, data_len);
};

int VFSFile::read(void *buffer, uint32_t buffer_len) {
	if(!current_data_block) return NULL;
	int err = 0;
	uint32_t readed_byte = 0;
	//read the requested byte size
	if((err = storage_driver_ptr->read(current_data_block, buffer, buffer_len, readed_byte))){
		return err;
	}
	return readed_byte;
}

