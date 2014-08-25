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


using namespace chaos::data_service::index_system;
namespace vfs=chaos::data_service::vfs;

StageDataVFileScanner::StageDataVFileScanner(vfs::VFSManager *_vfs_manager,
											 index_system::IndexDriver *_index_driver,
											 vfs::VFSStageReadableFile *_working_stage_file):
DataPackScanner(_vfs_manager,
				_index_driver,
				_working_stage_file),
last_hb_on_vfile(0) {
	
}

StageDataVFileScanner::~StageDataVFileScanner() {
}


boost::shared_ptr<DataFileInfo> StageDataVFileScanner::getWriteableFileForDID(const std::string& did) {
	boost::shared_ptr<DataFileInfo> result;
	if(map_did_data_file.count(did)) {
		result = map_did_data_file[did];
	} else {
		result.reset(new DataFileInfo());
		
		//we need to create a new data file for new readed unique identifier
		if(!vfs_manager->getWriteableDataFile(did, &result->data_file_ptr) && result->data_file_ptr){
			//we need to ensure that datablock is not null for determinate correct first block location
			result->data_file_ptr->ensureDatablockAllocated();
			
			//insert file into the hasmap
			map_did_data_file.insert(make_pair(did, result));
		}else {
			StageDataVFileScannerLERR_ << "Error creating data file for " << did;
		}
	}
	return result;
}

int StageDataVFileScanner::processDataPack(const bson::BSONObj& data_pack) {
	int err = 0;
	uint64_t cur_ts = chaos::TimingUtil::getTimeStamp();
	
	if(!data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID) ||
	   !data_pack.hasField(chaos::DataPackKey::CS_CSV_CU_ID)) {
		StageDataVFileScannerLDBG_ << "Current scanned data pack doesn't have required field so we skip it";
		return 0;
	}
	
	//get values for key that are mandatory for default index
	string did = data_pack.getField(chaos::DataPackKey::CS_CSV_CU_ID).String();
	
	//get file for unique id
	boost::shared_ptr<DataFileInfo> data_file_info = getWriteableFileForDID(did);
	if(!data_file_info->data_file_ptr) {
		StageDataVFileScannerLERR_ << "No vfs got for unique id " << did;
	} else if((err = data_file_info->data_file_ptr->write((void*)data_pack.objdata(), data_pack.objsize()))) {
		StageDataVFileScannerLERR_ << "Error writing datafile "<< err;
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