/*
 *	PosixStorageDriver.cpp
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

#include "PosixStorageDriver.h"

#include <iostream>
#include <sys/stat.h>

#include <boost/format.hpp>

#define PSD_CASTED_SETTING static_cast<PosixStorageDriverSetting*>(setting)

#define PosixStorageDriver_LOG_HEAD "[PosixStorageDriver] - "

#define PSDLAPP_ LAPP_ << PosixStorageDriver_LOG_HEAD
#define PSDLDBG_ LDBG_ << PosixStorageDriver_LOG_HEAD << __FUNCTION__ << " - "
#define PSDLERR_ LERR_ << PosixStorageDriver_LOG_HEAD << __FUNCTION__ << " - "


using namespace chaos::data_service::storage_system;

//---------------------------------------------------------------------------

PosixStorageDriver::PosixStorageDriver(std::string alias):StorageDriver(alias) {
	
}

PosixStorageDriver::~PosixStorageDriver() {
	
}

//! init
void PosixStorageDriver::init(void *init_data) throw (chaos::CException) {
	StorageDriver::init(init_data);
	if(!PSD_CASTED_SETTING->fsd_domain_path.size()) throw CException(-1, "No root path setupped", __PRETTY_FUNCTION__);
	
	PSDLAPP_ << "checking configuration";
	//set the data and stage path
	path_data_area = boost::str(boost::format("%1%/%2%") % PSD_CASTED_SETTING->fsd_domain_path % SD_DATA_AREA_ALIAS);
	path_stage_area = boost::str(boost::format("%1%/%2%") % PSD_CASTED_SETTING->fsd_domain_path % SD_STAGE_AREA_ALIAS);
	
	PSDLAPP_<< "Posix data area path -> " << path_data_area;
	PSDLAPP_<< "Posix stage area path -> " << path_stage_area;
}

//! deinit
void PosixStorageDriver::deinit() throw (chaos::CException) {
	
}

std::string PosixStorageDriver::getAbsolutePath(std::string vfs_path) {
	return boost::str(boost::format("%1%/%2%") % setting->fs_root_path % vfs_path);
}

//! Open a block
int PosixStorageDriver::openBlock(chaos_vfs::DataBlock *data_block, unsigned int flags) {
	CHAOS_ASSERT(data_block)
	boost::filesystem::fstream *fstream_ptr = NULL;
	try {
		boost::filesystem::path _path = data_block->fs_path;
		boost::filesystem::file_status _status = boost::filesystem::status(_path);
		if(boost::filesystem::is_directory(_status)) {
			return -1;
		}
		
		std::ios_base::openmode mode = std::ios_base::binary;
		if(flags & chaos_vfs::block_flag::BlockFlagWriteble) {
			mode &= std::ios_base::out;
		}
		
		if(flags & chaos_vfs::block_flag::BlockFlagReadeable) {
			mode &= std::ios_base::in;
		}

		//check on the max size
		
		data_block->driver_private_data = fstream_ptr = new boost::filesystem::fstream(_path, mode);
	} catch (boost::filesystem::filesystem_error &e) {
		if(fstream_ptr) delete(fstream_ptr);
		//delete the allocated stream
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

// open a block of a determinated type with
int PosixStorageDriver::openBlock(std::string vfs_path, unsigned int flags, chaos_vfs::DataBlock **data_block) {
	CHAOS_ASSERT(data_block)
	boost::filesystem::path _path = getAbsolutePath(vfs_path);
	boost::filesystem::fstream *ofs = NULL;
	
	//reset memory (if the user use an allocated handle here there is memory leak)
	*data_block = NULL;
	
	try {
		boost::filesystem::file_status _status = boost::filesystem::status(_path);
		
		if(boost::filesystem::is_directory(_status)) {
			//a directory is present with the same name
			return -1;
		}
		
		if(boost::filesystem::is_symlink(_status)) {
			//a directory is present with the same name
			return -2;
		}
		
		//we can create a file that match a new data block
		
		
		std::ios_base::openmode mode = std::ios_base::binary;
		if(flags & chaos_vfs::block_flag::BlockFlagWriteble) {
			mode &= std::ios_base::out;
		}
		
		if(flags & chaos_vfs::block_flag::BlockFlagReadeable) {
			mode &= std::ios_base::in;
		}
		
		ofs = new boost::filesystem::fstream(_path, mode);
		
		//no we can create the block
		*data_block = getNewDataBlock(_path.string().c_str());
		(*data_block)->driver_private_data = ofs;
		(*data_block)->flags = flags;
		(*data_block)->invalidation_timestamp = 0;
		(*data_block)->max_reacheable_size = 0;
	}catch (boost::filesystem::filesystem_error &e) {
		//delete the allcoated stream
		if(*data_block) {
			disposeDataBlock(*data_block);
			*data_block = NULL;
		} else if(ofs) {
			delete(ofs);
		}
		PSDLERR_ << e.what() << std::endl;
		return -3;
	}
	return 0;
}

//! return the block current size
int PosixStorageDriver::getBlockSize(chaos_vfs::DataBlock *data_block) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(data_block->driver_private_data)
	boost::filesystem::fstream *fstream_ptr = static_cast<boost::filesystem::fstream *>(data_block->driver_private_data);
	try {
		//write data
		fstream_ptr->seekg(0, fstream_ptr->end);
		data_block->current_size = (uint32_t)fstream_ptr->tellg();
		fstream_ptr->seekg(0, fstream_ptr->beg);
	}catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

//! close the block of data
int PosixStorageDriver::closeBlock(chaos_vfs::DataBlock *data_block) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(data_block->driver_private_data)
	
	try {
		static_cast<boost::filesystem::fstream *>(data_block->driver_private_data)->close();
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		//return -1;
	}
	disposeDataBlock(data_block);
	return 0;
}

//! return all block of data found on the path, acocrding to the type
int PosixStorageDriver::listBlock(std::string vfs_path, std::vector<chaos_vfs::DataBlock*>& bloks_found) {
	boost::filesystem::path _path = getAbsolutePath(vfs_path);
	try {
		boost::filesystem::file_status _status = boost::filesystem::status(_path);
		
		if(!boost::filesystem::is_directory(_status)) {
			//path is not a directory
			return -1;
		}
		//scan folder content
		boost::filesystem::directory_iterator end_itr;
		for ( boost::filesystem::directory_iterator itr( _path );
			 itr != end_itr;
			 ++itr ) {
			chaos_vfs::DataBlock *data_block = getNewDataBlock(itr->path().string());
			data_block->flags = chaos_vfs::block_flag::BlockFlagNone;
			data_block->invalidation_timestamp = 0;
			data_block->max_reacheable_size = 0;
			bloks_found.push_back(data_block);
		}
	} catch (boost::filesystem::filesystem_error &e) {
		bloks_found.clear();
		PSDLERR_ << e.what() << std::endl;
		return -2;
	}
	return 0;
}

//! write an amount of data into a DataBlock
int PosixStorageDriver::write(chaos_vfs::DataBlock *data_block, void * data, uint32_t data_len) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(data_block->driver_private_data)
	try {
		//write data
		static_cast<boost::filesystem::fstream *>(data_block->driver_private_data)->write((const char*)data, data_len);
		
		//update current size field
		data_block->current_size += data_len;
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

//! read an amount of data from a DataBlock
int PosixStorageDriver::read(chaos_vfs::DataBlock *data_block, uint64_t offset, void * * data, uint32_t& data_len) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(data_block->driver_private_data)
	try {
		boost::filesystem::fstream *fstream_ptr = static_cast<boost::filesystem::fstream *>(data_block->driver_private_data);
		fstream_ptr->read((char*)*data, data_len);
		if(!*fstream_ptr) {
			return -1;
		}
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;

}
