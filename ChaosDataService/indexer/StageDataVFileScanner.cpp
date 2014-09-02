/*
 *	StageDataVFileScanner.cpp
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

#include "StageDataVFileScanner.h"
#include "../vfs/VFSManager.h"

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/endianess.h>

#define StageDataVFileScanner_LOG_HEAD "[StageDataVFileScanner] - "

#define StageDataVFileScannerLAPP_ LAPP_ << StageDataVFileScanner_LOG_HEAD
#define StageDataVFileScannerLDBG_ LDBG_ << StageDataVFileScanner_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataVFileScannerLERR_ LERR_ << StageDataVFileScanner_LOG_HEAD


using namespace chaos::data_service;
using namespace chaos::data_service::indexer;
namespace vfs=chaos::data_service::vfs;

StageDataVFileScanner::StageDataVFileScanner(vfs::VFSManager *_vfs_manager,
											 db_system::DBDriver *_db_driver,
											 vfs::VFSStageReadableFile *_working_stage_file):
DataPackScanner(_vfs_manager,
				_db_driver,
				_working_stage_file),
last_hb_on_vfile(0) {
	
}

StageDataVFileScanner::~StageDataVFileScanner() {
	closeAllDatafile();
}


boost::shared_ptr<DataFileInfo> StageDataVFileScanner::getWriteableFileForDID(const std::string& did) {
	boost::shared_ptr<DataFileInfo> result;
	if(map_did_data_file.count(did)) {
		result = map_did_data_file[did];
	} else {
		result.reset(new DataFileInfo());
		
		//we need to create a new data file for new readed unique identifier
		if(!vfs_manager->getWriteableDataFile(did, &result->data_file_ptr) && result->data_file_ptr){
			//insert file into the hasmap
			map_did_data_file.insert(make_pair(did, result));
		}else {
			StageDataVFileScannerLERR_ << "Error creating data file for " << did;
		}
	}
	//we need to ensure that datablock is not null for determinate correct first block location
	if(result.get())result->data_file_ptr->prefetchData();
	
	return result;
}

int StageDataVFileScanner::closeAllDatafile() {
	int err = 0;
	//scan as endded with error so we need to clean
	for(std::map<std::string, shared_ptr<DataFileInfo> >::iterator it =  map_did_data_file.begin();
		it != map_did_data_file.end();
		it++){
		StageDataVFileScannerLAPP_ << "closing datafile: " << it->second->data_file_ptr->getVFSFileInfo()->vfs_fpath;
		if((err = vfs_manager->releaseFile(it->second->data_file_ptr))){
			StageDataVFileScannerLERR_ << "error closing data file";
		}
	}
	map_did_data_file.clear();
	return err;
}

int StageDataVFileScanner::startScanHandler() {
	return 0;
}

int StageDataVFileScanner::processDataPack(const bson::BSONObj& data_pack) {
	int err = 0;
	uint64_t cur_ts = chaos::TimingUtil::getTimeStamp();
	
	if(!data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID) ||
	   !data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID)) {
		StageDataVFileScannerLDBG_ << "Current scanned data pack doesn't have required field so we skip it";
		return 0;
	}
	//collect idnex information
	db_system::DataPackIndex new_data_pack_index;
	
	
	//get values for key that are mandatory for default index
	new_data_pack_index.did = data_pack.getField(chaos::DataPackKey::CS_CSV_CU_ID).String();
	new_data_pack_index.acquisition_ts = data_pack.getField(chaos::DataPackKey::CS_CSV_TIME_STAMP).numberLong();
	
	//get file for unique id
	boost::shared_ptr<DataFileInfo> data_file_info = getWriteableFileForDID(new_data_pack_index.did);
	if(!data_file_info->data_file_ptr) {
		StageDataVFileScannerLERR_ << "No vfs got for unique id " << new_data_pack_index.did;
		return -1;
	}
	//! write get data file phase
	if((err = working_data_file->appendValueToJournal("gdf"))){
		StageDataVFileScannerLERR_ << "Error writing gdf tag";
		return err;
	}

	//write packet to his virtaul data file
	new_data_pack_index.dst_location = data_file_info->data_file_ptr->getCurrentFileLocation();
	if(!new_data_pack_index.dst_location.isValid()) {
		//strange error because datablock is null
		StageDataVFileScannerLERR_ << "Error on retrieve datafile offset";
		return -2;
	}
	//! write journal
	if((err = working_data_file->appendValueToJournal("gloc"))){
		StageDataVFileScannerLERR_ << "Error writing gloc tag";
		return err;
	}

	//write index for the datapack on database
	if((err = db_driver->idxAddDataPackIndex(new_data_pack_index))) {
		if(err == 11000) {
			StageDataVFileScannerLERR_ << "Index already present";
		} else {
			StageDataVFileScannerLERR_ << "Error writing index to database";
			return err;
		}
	}
	//! write journal
	if((err = working_data_file->appendValueToJournal("widx"))){
		StageDataVFileScannerLERR_ << "Error writing widx tag";
		return err;
	}
	
	//write data pack on data file
	if((err = data_file_info->data_file_ptr->write((void*)data_pack.objdata(), data_pack.objsize()))) {
		StageDataVFileScannerLERR_ << "Error writing datafile "<< err;
	}
	//! write journal
	if((err = working_data_file->appendValueToJournal("wpack"))){
		StageDataVFileScannerLERR_ << "Error writing wpac tag";
		return err;
	}
	
	//give heartbeat on datafile
	if(cur_ts > last_hb_on_vfile + 2000) {
		if((err = data_file_info->data_file_ptr->giveHeartbeat(cur_ts))) {
			StageDataVFileScannerLERR_ << "Error writing datafile "<< err;
		}
		last_hb_on_vfile = cur_ts;
	}
	return err;
}

int StageDataVFileScanner::endScanHandler(int end_scan_error) {
	int err = 0;
	if(end_scan_error) {
		StageDataVFileScannerLAPP_ << "An error has occurred during scanner, close all datafile";
		closeAllDatafile();
	} else {
		//all is gone weel but we need to do some mantainance
		//scan as endded with error so we need to clean
		for(std::map<std::string, shared_ptr<DataFileInfo> >::iterator it =  map_did_data_file.begin();
			it != map_did_data_file.end();
			it++){
			
			// we need to maintin datablock on to long idle datafile so datablock no more valid(time/size) are closed
			// and prefetchData call ensure that one valida databloc is allocated if mantainance close an old one.
			StageDataVFileScannerLAPP_ << "Mantainance for datafile: " << it->second->data_file_ptr->getVFSFileInfo()->vfs_fpath;
			if((err = it->second->data_file_ptr->mantain(vfs::data_block_state::DataBlockStateQuerable))) {
				StageDataVFileScannerLERR_ << "error mantaining datafile " << it->second->data_file_ptr->getVFSFileInfo()->vfs_fpath;
			}
		}

	}
	return err;
}