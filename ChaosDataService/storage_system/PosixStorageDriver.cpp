//
//  PosixStorageDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "PosixStorageDriver.h"

#include <iostream>

#include <boost/format.hpp>

#define PSD_CASTED_SETTING static_cast<PosixStorageDriverSetting*>(setting)

#define PosixStorageDriver_LOG_HEAD "[PosixStorageDriver] - "

#define PSDLAPP_ LAPP_ << PosixStorageDriver_LOG_HEAD
#define PSDLDBG_ LDBG_ << PosixStorageDriver_LOG_HEAD << __FUNCTION__ << " - "
#define PSDLERR_ LERR_ << PosixStorageDriver_LOG_HEAD << __FUNCTION__ << " - "


using namespace chaos::data_service::storage_system;

PosixDataBlock::PosixDataBlock():fstream(NULL) {
	
}

PosixDataBlock::PosixDataBlock(boost::filesystem::fstream *_fstream):fstream(_fstream) {
	
}

PosixDataBlock::~PosixDataBlock() {
	if(fstream) delete(fstream);
}

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

//! Open a block
int PosixStorageDriver::openBlock(chaos_vfs::DataBlock *data_block) {
	CHAOS_ASSERT(data_block)
	try {
		std::ios_base::openmode mode = std::ios_base::binary;
		if(data_block->flags & chaos_vfs::block_flag::BlockFlagWriteble) {
			mode &= std::ios_base::out;
		}
		
		if(data_block->flags & chaos_vfs::block_flag::BlockFlagReadeable) {
			mode &= std::ios_base::in;
		}
		
		boost::filesystem::path _path = data_block->path;
		boost::filesystem::file_status _status = boost::filesystem::status(_path);
		if(boost::filesystem::is_directory(_status)) {
			return -1;
		}
		
		boost::filesystem::space_info _space = boost::filesystem::space(_path);
		//no we can create the block
		
		((PosixDataBlock*)data_block)->fstream = new boost::filesystem::fstream(_path, mode);
		
		((PosixDataBlock*)data_block)->fstream->seekg (0, ((PosixDataBlock*)data_block)->fstream->end);
		uint32_t length = ((PosixDataBlock*)data_block)->fstream->tellg();
		((PosixDataBlock*)data_block)->fstream->seekg (0, ((PosixDataBlock*)data_block)->fstream->beg);
	} catch (boost::filesystem::filesystem_error &e) {
		//delete the allcoated stream
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

// open a block of a determinated type with
int PosixStorageDriver::openBlock(chaos_vfs::block_type::BlockType type, std::string path, unsigned int flags, chaos_vfs::DataBlock **data_block) {
	CHAOS_ASSERT(data_block)
	boost::filesystem::path _path;
	boost::filesystem::fstream *ofs = NULL;
	
	//reset memory (if the user use an allocated handle here there is memory leak)
	*data_block = NULL;
	
	try {
		switch (type) {
			case vfs::block_type::BlockTypeData:
				_path = boost::str(boost::format("%1%/%2%") % path_data_area % path);
				break;
				
			case vfs::block_type::BlockTypeStage:
				_path = boost::str(boost::format("%1%/%2%") % path_stage_area % path);
				break;
		}
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
		*data_block = new PosixDataBlock(ofs);
		(*data_block)->type = type;
		(*data_block)->flags = flags;
		(*data_block)->path = _path.string();
		(*data_block)->invalidation_timestamp = 0;
		(*data_block)->max_reacheable_size = 0;
	}catch (boost::filesystem::filesystem_error &e) {
		//delete the allcoated stream
		if(*data_block) {
			delete(*data_block);
			*data_block = NULL;
		} else if(ofs) {
			delete(ofs);
		}
		PSDLERR_ << e.what() << std::endl;
		return -3;
	}
	return 0;
}

//! close the block of data
int PosixStorageDriver::closeBlock(chaos_vfs::block_type::BlockType *data_block) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(((PosixDataBlock*)data_block)->fstream)
	try {
		((PosixDataBlock*)data_block)->fstream->close();
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

//! return all block of data found on the path, acocrding to the type
int PosixStorageDriver::listBlock(chaos_vfs::block_type::BlockType type, std::string path, boost::ptr_vector<chaos_vfs::DataBlock>& bloks_found) {
	boost::filesystem::path _path;
	try {
		switch (type) {
			case vfs::block_type::BlockTypeData:
				_path = boost::str(boost::format("%1%/%2%") % path_data_area % path);
				break;
				
			case vfs::block_type::BlockTypeStage:
				_path = boost::str(boost::format("%1%/%2%") % path_stage_area % path);
				break;
		}
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
			chaos_vfs::DataBlock *data_block = new PosixDataBlock();
			data_block->type = type;
			data_block->flags = chaos_vfs::block_flag::BlockFlagNone;
			data_block->path = itr->path().string();
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
int PosixStorageDriver::write(chaos_vfs::block_type::BlockType *data_block, void * data, uint32_t data_len) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(((PosixDataBlock*)data_block)->fstream)
	try {
		//write data
		((PosixDataBlock*)data_block)->fstream->write((const char*)data, data_len);
		
		//update current size field
		((PosixDataBlock*)data_block)->current_size += data_len;
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;
}

//! read an amount of data from a DataBlock
int PosixStorageDriver::read(chaos_vfs::block_type::BlockType *data_block, uint64_t offset, void * * data, uint32_t *data_len) {
	CHAOS_ASSERT(data_block)
	CHAOS_ASSERT(((PosixDataBlock*)data_block)->fstream)
	try {
		//((PosixDataBlock*)data_block)->fstream->read(const char*)data, data_len);
	} catch (boost::filesystem::filesystem_error &e) {
		PSDLERR_ << e.what() << std::endl;
		return -1;
	}
	return 0;

}
