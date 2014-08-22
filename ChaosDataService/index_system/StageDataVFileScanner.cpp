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
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/endianess.h>

#define StageDataVFileScanner_LOG_HEAD "[StageDataVFileScanner] - "

#define StageDataVFileScannerLAPP_ LAPP_ << StageDataVFileScanner_LOG_HEAD
#define StageDataVFileScannerLDBG_ LDBG_ << StageDataVFileScanner_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataVFileScannerLERR_ LERR_ << StageDataVFileScanner_LOG_HEAD


using namespace chaos::data_service::index_system;
namespace vfs=chaos::data_service::vfs;

StageDataVFileScanner::StageDataVFileScanner(vfs::VFSManager *_vfs_manager,
											 vfs::VFSStageReadableFile *_working_stage_file):
vfs_manager(_vfs_manager),
working_stage_file(_working_stage_file),
data_buffer(NULL),
curret_data_buffer_len(0) {
	
	//for first time make place for only BSON header
	data_buffer = malloc(curret_data_buffer_len = 4);
}

StageDataVFileScanner::~StageDataVFileScanner() {
	if(data_buffer) free(data_buffer);
}

std::string StageDataVFileScanner::getScannedVFSPath() {
	return working_stage_file->getVFSFileInfo()->vfs_fpath;
}

void StageDataVFileScanner::grow(uint32_t new_size) {
	if(curret_data_buffer_len < new_size) {
		data_buffer = realloc(data_buffer, (curret_data_buffer_len = new_size));
	}
}

vfs::VFSDataWriteableFile *StageDataVFileScanner::getWriteableFileForDID(const std::string& did) {
	vfs::VFSDataWriteableFile *vfile = NULL;
	if(map_did_data_file.count(did)) {
		vfile = map_did_data_file[did];
	} else {
		//we need to create a new data file for new readed unique identifier
		if(!vfs_manager->getWriteableDataFile(did, &vfile) && vfile){
			map_did_data_file.insert(make_pair(did, vfile));
		}else {
			StageDataVFileScannerLERR_ << "Error creating data file for " << did;
		}
	}
	return vfile;
}

bool StageDataVFileScanner::processDataPack(const bson::BSONObj& data_pack) {
	if(!data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID) ||
	   data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID)) {
		StageDataVFileScannerLDBG_ << "Current scanned data pack doesn't have required field: " << data_pack.toString();
		return false;
	}
	//get values for key that are mandatory for default index
	std::string did = data_pack.getField(chaos::DataPackKey::CS_CSV_CU_ID).String();
	uint64_t	dts = data_pack.getField(chaos::DataPackKey::CS_CSV_TIME_STAMP).numberLong();
	
	//get file for unique id
	vfs::VFSDataWriteableFile *did_vfile = getWriteableFileForDID(did);
	if(!did_vfile) {
		//strange error because datablock is null
		StageDataVFileScannerLERR_ << "No vfs got for unique id " << did;
		return false;
	}
	//write packet to his virtaul data file
	int64_t current_offset = did_vfile->getCurrentOffSet();
	if(current_offset == -1) {
		//strange error because datablock is null
		StageDataVFileScannerLERR_ << "Error on retrieve datafile offset";
		return false;
	}
	
	//write pack on file
	if(did_vfile->write((void*)data_pack.objdata(), data_pack.objsize())) {
		StageDataVFileScannerLERR_ << "Error writing datafile";
		return false;
	}
	
	return true;
}

#define BREAK_ON_NO_DATA_READED \
if(err <= 0) { \
work = false; \
break; \
} \

#define BSON_HEADER_SIZE 4
// scan an entire block of the stage file
int StageDataVFileScanner::scan() {
	int err = 0;
	if(!data_buffer) return -1;
	bool work = true;
	while(work) {
		//read the header
		err = working_stage_file->read(data_buffer, BSON_HEADER_SIZE);
		BREAK_ON_NO_DATA_READED
		
		//get the bson size to read
		uint32_t bson_size = FROM_LITTLE_ENDNS(int32_t, data_buffer, 0);
		
		//check if we need to expand the buffer
		grow(bson_size);
		
		//read all bson
		err = working_stage_file->read((static_cast<char*>(data_buffer)+4), bson_size-4);
		BREAK_ON_NO_DATA_READED
		
		processDataPack(bson::BSONObj(static_cast<const char*>(data_buffer)));
	}
	return 0;
}