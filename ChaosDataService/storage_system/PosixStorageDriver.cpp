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

// open a block of a determinated type with
int PosixStorageDriver::openBlock(chaos_vfs::block_type::BlockType type, std::string path, unsigned int flags, chaos_vfs::DataBlock **data_block) {
	boost::filesystem::path _path;
	boost::filesystem::fstream *ofs = NULL;
	
	if(!data_block) return -1;
	
	//reset memory (if the user use an allocated handle here there is memory leak)
	*data_block = NULL;
	
	try 	{
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
			return -2;
		}
		
		if(boost::filesystem::is_symlink(_status)) {
			//a directory is present with the same name
			return -3;
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
		(*data_block)->path = path;
		(*data_block)->invalidation_timestamp = 0;
		(*data_block)->max_reacheable_size = 0;
	}catch (boost::filesystem::filesystem_error &e) {
		//delete the allcoated stream
		if(*data_block) {
			delete(*data_block);
			*data_block = NULL;
		}else if(ofs) {
			delete(ofs);
		}
		
		
		
		PSDLERR_ << e.what() << std::endl;
		return -4;
	}
	return 0;
}

//! close the block of data
int PosixStorageDriver::closeBlock(chaos_vfs::block_type::BlockType *data_block) {
	return 0;
}

//! return all block of data found on the path, acocrding to the type
int PosixStorageDriver::listBlock(chaos_vfs::block_type::BlockType type, std::string path, boost::ptr_vector<chaos_vfs::DataBlock>& bloks_found) {
	return 0;
}

//! write an amount of data into a DataBlock
int PosixStorageDriver::write(chaos_vfs::block_type::BlockType *data_block, void * data, uint32_t data_len) {
	return 0;
}

//! read an amount of data from a DataBlock
int PosixStorageDriver::read(chaos_vfs::block_type::BlockType *data_block, uint64_t offset, void * * data, uint32_t *data_len) {
	return 0;
}
