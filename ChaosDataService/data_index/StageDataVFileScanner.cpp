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

#include <chaos/common/utility/endianess.h>

#define StageDataVFileScanner_LOG_HEAD "[StageDataVFileScanner] - "

#define StageDataVFileScannerLAPP_ LAPP_ << StageDataVFileScanner_LOG_HEAD
#define StageDataVFileScannerLDBG_ LDBG_ << StageDataVFileScanner_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataVFileScannerLERR_ LERR_ << StageDataVFileScanner_LOG_HEAD


using namespace chaos::data_service::data_index;

StageDataVFileScanner::StageDataVFileScanner(vfs::VFSStageReadableFile *_stage_file):
stage_file(_stage_file),
data_buffer(NULL),
curret_data_buffer_len(0) {
	
	//for first time make place for only BSON header
	data_buffer = malloc(curret_data_buffer_len = 4);
}

StageDataVFileScanner::~StageDataVFileScanner() {
	if(data_buffer) free(data_buffer);
}

std::string StageDataVFileScanner::getScannedVFSPath() {
	return stage_file->getVFSFileInfo()->vfs_fpath;
}

void StageDataVFileScanner::grow(uint32_t new_size) {
	if(curret_data_buffer_len < new_size) {
		data_buffer = realloc(data_buffer, (curret_data_buffer_len = new_size));
	}
}

void StageDataVFileScanner::processDataPack(bson::BSONObj data_pack) {
	StageDataVFileScannerLDBG_ << data_pack.toString();
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
		err = stage_file->read(data_buffer, BSON_HEADER_SIZE);
		BREAK_ON_NO_DATA_READED
		
		uint32_t bson_size = FROM_LITTLE_ENDNS(int32_t, data_buffer, 0);
		
		//check if we need to expand the buffer
		grow(bson_size);
		
		//read all bson
		err = stage_file->read((static_cast<char*>(data_buffer)+4), bson_size-4);
		BREAK_ON_NO_DATA_READED
		
		processDataPack(bson::BSONObj(static_cast<const char*>(data_buffer)));
	}
	return 0;
}