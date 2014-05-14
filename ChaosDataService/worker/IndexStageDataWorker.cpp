/*
 *	IndexStageDataWorker.cpp
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

#include "IndexStageDataWorker.h"

using namespace chaos::data_service;
using namespace chaos::data_service::worker;

IndexStageDataWorker::IndexStageDataWorker(vfs::VFSManager *_vfs_manager_instance) {
	
}

IndexStageDataWorker::~IndexStageDataWorker() {
	
}

void IndexStageDataWorker::init(void *init_data) throw (chaos::CException) {
	
}

void IndexStageDataWorker::deinit() throw (chaos::CException) {
	
}

void IndexStageDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	IndexStageDataJob *dw_job_ptr = reinterpret_cast<IndexStageDataJob*>(job_info);
}