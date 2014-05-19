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

#define StageDataVFileScanner_LOG_HEAD "[StageDataVFileScanner] - "

#define StageDataVFileScannerLAPP_ LAPP_ << StageDataVFileScanner_LOG_HEAD
#define StageDataVFileScannerLDBG_ LDBG_ << StageDataVFileScanner_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataVFileScannerLERR_ LERR_ << StageDataVFileScanner_LOG_HEAD

using namespace chaos::data_service::data_index;

StageDataVFileScanner::StageDataVFileScanner(vfs::VFSStageReadableFile *_stage_file):stage_file(_stage_file) {
	
}

StageDataVFileScanner::~StageDataVFileScanner() {
	
}

std::string StageDataVFileScanner::getScannedVFSPath() {
	return stage_file->getVFSFileInfo()->vfs_fpath;
}

// scan an entire block of the stage file
int StageDataVFileScanner::scan() {
	return 0;
}