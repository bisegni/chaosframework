/*
 *	DataPackScanner.cpp
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

#include "DataPackScanner.h"
#include "../vfs/VFSManager.h"
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/endianess.h>

#define DataPackScanner_LOG_HEAD "[DataPackScanner] - "

#define DataPackScannerLAPP_ LAPP_ << DataPackScanner_LOG_HEAD
#define DataPackScannerLDBG_ LDBG_ << DataPackScanner_LOG_HEAD << __FUNCTION__ << " - "
#define DataPackScannerLERR_ LERR_ << DataPackScanner_LOG_HEAD


using namespace chaos::data_service::indexer;
namespace vfs=chaos::data_service::vfs;

DataPackScanner::DataPackScanner(vfs::VFSManager *_vfs_manager,
								 db_system::DBDriver *_db_driver,
								 vfs::VFSFile *_working_data_file):
vfs_manager(_vfs_manager),
db_driver(_db_driver),
working_data_file(_working_data_file),
data_buffer(NULL),
curret_data_buffer_len(0) {
	
	//for first time make place for only BSON header
	data_buffer = malloc(curret_data_buffer_len = 4);
}

DataPackScanner::~DataPackScanner() {
	if(data_buffer) free(data_buffer);
	
}

std::string DataPackScanner::getScannedVFSPath() {
	return working_data_file->getVFSFileInfo()->vfs_fpath;
}

void DataPackScanner::grow(uint32_t new_size) {
	if(curret_data_buffer_len < new_size) {
		data_buffer = realloc(data_buffer, (curret_data_buffer_len = new_size));
	}
}

#define BREAK_ON_NO_DATA_READED \
if(err <= 0) { \
break; \
} \

#define BSON_HEADER_SIZE 4
// scan an entire block of the stage file
int DataPackScanner::scan() {
	int err = 0;
	bool has_data = false;
	if(!data_buffer) return -1;
	
	//call start handler
	if((err = startScanHandler())) return err;
	
	if((err = working_data_file->prefetchData())) {
		DataPackScannerLERR_ << "error " << err << "prefetching data on working file " << working_data_file->getVFSFileInfo()->vfs_fpath;
	} else if((err = working_data_file->hasData(has_data))) {
		DataPackScannerLERR_ << "errr checking if data is available on" << working_data_file->getVFSFileInfo()->vfs_fpath;
	}
	
	if (!has_data) {
		DataPackScannerLAPP_ << "no data to read on " << working_data_file->getVFSFileInfo()->vfs_fpath;
		return 0;
	}
	
	while(!err) {
		if((err = working_data_file->syncCurrentOffsetToJournal())) {
			DataPackScannerLERR_ << "error sinking work location to journal";
			break;
		}
		
		//! open journal tag for the reading
		working_data_file->openKeyToJournal("process");
				
		//read the header
		err = working_data_file->read(data_buffer, BSON_HEADER_SIZE);
		BREAK_ON_NO_DATA_READED
		
		//! write first read phase
		working_data_file->appendValueToJournal("rh");
		
		//get the bson size to read
		uint32_t bson_size = FROM_LITTLE_ENDNS(int32_t, data_buffer, 0);
		
		//check if we need to expand the buffer
		grow(bson_size);

		//read all bson
		err = working_data_file->read((static_cast<char*>(data_buffer)+4), bson_size-4);
		BREAK_ON_NO_DATA_READED
		
		//! write read second phase
		working_data_file->appendValueToJournal("rd");
		
		//! write read phase
		working_data_file->appendValueToJournal("p");
		if((err = processDataPack(bson::BSONObj(static_cast<const char*>(data_buffer))))){
			DataPackScannerLERR_ << "error processing data";
		}
		//! open journal tag for the reading
		working_data_file->closeKeyToJournal("process");
	}
	
	//call end scanner handler
	return endScanHandler(err);
}
