/*
 *	StageDataConsumer.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "StageDataConsumer.h"

#include <unistd.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::data_service;

#define StageDataConsumer_LOG_HEAD "[StageDataConsumer] - "

#define StageDataConsumerLAPP_ LAPP_ << StageDataConsumer_LOG_HEAD
#define StageDataConsumerLDBG_ LDBG_ << StageDataConsumer_LOG_HEAD << __FUNCTION__ << " - "
#define StageDataConsumerLERR_ LERR_ << StageDataConsumer_LOG_HEAD

#define DISPOSE_SCANNER_INFO(s) \
vfs_manager_ptr->releaseFile(s->scanner->working_data_file);\
delete (s->scanner);\
delete s;\
s=NULL;


#define LOCK_SCANNER_INFO(s) \
boost::unique_lock<boost::mutex> l(s->mutex_on_scan);

StageDataConsumer::StageDataConsumer(vfs::VFSManager *_vfs_manager_ptr,
									 db_system::DBDriver *_db_driver_ptr,
									 ChaosDataServiceSetting *_settings):
settings(_settings),
work_on_stage(false),
vfs_manager_ptr(_vfs_manager_ptr),
db_driver_ptr(_db_driver_ptr) {}

StageDataConsumer::~StageDataConsumer() {}

void StageDataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!settings)  throw chaos::CException(-1, "No setting provided", __FUNCTION__);
	
	//add answer worker
	StageDataConsumerLAPP_ << "Allocating index driver";
}

void StageDataConsumer::start() throw (chaos::CException) {
	time_to_check_timeouted_stage_file = 0;
	StageDataConsumerLAPP_ << "Start stage scanner thread";
	work_on_stage = true;
	for(int idx = 0; idx < settings->indexer_worker_num; idx++) {
		thread_scanners.add_thread(new boost::thread(boost::bind(&StageDataConsumer::scanStage, this)));
	}
}

void StageDataConsumer::stop() throw (chaos::CException) {
	StageDataConsumerLAPP_ << "Stop stage scanner thread";
	work_on_stage = false;
	thread_scanners.join_all();
	
}

void StageDataConsumer::deinit() throw (chaos::CException) {
	
}

void StageDataConsumer::scanStage() {
	StageDataConsumerLAPP_ << "Enter stage scanner thread";
	int err = 0;
	uint64_t curr_ts = 0;
	std::string indexable_stage_file_vfs_path;
	std::vector<std::string> current_stage_file;
	vfs::VFSStageReadableFile *stage_file_to_process = NULL;
	indexer::StageDataVFileScanner *indexer = new indexer::StageDataVFileScanner(vfs_manager_ptr, db_driver_ptr);
	//
    if(!indexer) {
        StageDataConsumerLDBG_ << "Error creating the indexer";
        return;
    }
	while(work_on_stage) {
		try {
			{
				boost::unique_lock<boost::mutex> l(mutex_timeout_check);
				if((curr_ts = chaos::TimingUtil::getTimeStamp()) > time_to_check_timeouted_stage_file + 60000) {
					StageDataConsumerLDBG_ << "Manage timeout datablock in aquiring state";
					//every minut start the check for the timeout datafile
					time_to_check_timeouted_stage_file = curr_ts;
					
					//pull all databloc in acquiring state that are in timeout
					//! to none to perform the indexing of the contained data
					vfs_manager_ptr->changeStateToTimeoutedDatablock(false,													//stage area
																	 60,													//add 60 second for valid until field check
																	 vfs::data_block_state::DataBlockStateAquiringData,		//get only block in acquiring state(and in timeout)
																	 vfs::data_block_state::DataBlockStateNone);			//set it to done
				
					
					//mantains scanner
					indexer->mantains();
				}
			}
			// get
			if((err = vfs_manager_ptr->getNextIndexableStageFileVFSPath(indexable_stage_file_vfs_path))) {
				StageDataConsumerLDBG_ << "Error retrieving the nex indexable vfile";
			} else if(indexable_stage_file_vfs_path.size()) {
				if((err = vfs_manager_ptr->getReadableStageFile(indexable_stage_file_vfs_path,
																&stage_file_to_process))) {
					//error gettin file
					StageDataConsumerLDBG_ << "Error getting vfs stage readable file ";
					continue;
				} else {
                    CHAOS_ASSERT(stage_file_to_process)
					StageDataConsumerLAPP_ << "Scan for index process the file ->"<< stage_file_to_process->getVFSFileInfo()->vfs_fpath;
					// scan one block of virtual file
					indexer->scan(stage_file_to_process);
					//mantains scanner
					indexer->mantains();
					
					StageDataConsumerLAPP_ << "End scanner for file ->"<< stage_file_to_process->getVFSFileInfo()->vfs_fpath;
				}
			} else {
				//no file has been found to scann so we can sleep a little bit
				boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
			}
		} catch (...) {
			StageDataConsumerLERR_ << "General exception and doesn need to occure";
		}
		if(stage_file_to_process) {
			vfs_manager_ptr->releaseFile(stage_file_to_process);
			stage_file_to_process = NULL;
		}
		//clean the path of last found file path
		indexable_stage_file_vfs_path.clear();
	}
    if(indexer) {
        StageDataConsumerLDBG_ << "Delete indexer";
        delete(indexer);
    }
	StageDataConsumerLAPP_ << "Leaving stage scanner thread";
}