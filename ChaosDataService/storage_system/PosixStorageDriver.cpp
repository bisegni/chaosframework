//
//  PosixStorageDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "PosixStorageDriver.h"

#include <boost/format.hpp>

#define PSD_CASTED_SETTING static_cast<PosixStorageDriverSetting*>(setting)

#define PosixStorageDriver_LOG_HEAD "[PosixStorageDriver] - "

#define PSDLAPP_ LAPP_ << PosixStorageDriver_LOG_HEAD
#define PSDLDBG_ LDBG_ << PosixStorageDriver_LOG_HEAD << __FUNCTION__ << " - "
#define PSDLERR_ LERR_ << PosixStorageDriver_LOG_HEAD


using namespace chaos::data_service::storage_system;

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
int PosixStorageDriver::openBlock(chaos_vfs::BlockType type, std::string path, unsigned int flag, chaos_vfs::DataBlock **data_block) {
	return 0;
}

//! close the block of data
int PosixStorageDriver::closeBlock(chaos_vfs::BlockType *data_block) {
	return 0;
}

//! return all block of data found on the path, acocrding to the type
int PosixStorageDriver::listBlock(chaos_vfs::BlockType type, std::string path, boost::ptr_vector<chaos_vfs::DataBlock>& bloks_found) {
	return 0;
}

//! write an amount of data into a DataBlock
int PosixStorageDriver::write(chaos_vfs::BlockType *data_block, void * data, uint32_t data_len) {
	return 0;
}

//! read an amount of data from a DataBlock
int PosixStorageDriver::read(chaos_vfs::BlockType *data_block, uint64_t offset, void * * data, uint32_t *data_len) {
	return 0;
}
