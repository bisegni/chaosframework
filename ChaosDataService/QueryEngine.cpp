/*
 *	QueryAnswerEngine.h
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

#include "QueryEngine.h"

using namespace chaos::data_service;

#define QueryEngine_LOG_HEAD "[QueryDataConsumer] - "

#define QEAPP_ LAPP_ << QueryEngine_LOG_HEAD
#define QEDBG_ LDBG_ << QueryEngine_LOG_HEAD << __FUNCTION__ << " - "
#define QEERR_ LERR_ << QueryEngine_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

#define QUERY_INFO "query on key:" << query->query.did << "(since: " << query->query.start_ts << " to: " << query->query.end_ts << ") has been submitted"

QueryEngine::QueryEngine(unsigned int _thread_pool_size,
						 vfs::VFSManager *_vfs_manager_ptr):
work_on_query(false),
thread_pool_size(_thread_pool_size), 
vfs_manager_ptr(_vfs_manager_ptr),
query_queue(1) {
	
}

QueryEngine::~QueryEngine() {
	
}

void QueryEngine::init(void *init_data)  throw(chaos::CException) {
	if(!vfs_manager_ptr) throw chaos::CException(-1, "No vfs manager setupped", __FUNCTION__);

}

void QueryEngine::start() throw(chaos::CException) {
	QEAPP_ << "Starting thread pool of size " << thread_pool_size;
	//add al thread to the pool
	work_on_query = true;
	for(int idx = 0; idx < thread_pool_size; idx++) {
		answer_thread_pool.add_thread(new boost::thread(boost::bind(&QueryEngine::process_query, this)));
	}
}

void QueryEngine::stop() throw(chaos::CException) {
	work_on_query = false;
	answer_thread_pool.join_all();
}

void QueryEngine::deinit() throw(CException) {
	
}

//execute a query and manage the result
void QueryEngine::executeQuery(DataCloudQuery *query) {
	if(!query) return;
	
	//get a new vfs query
	if(vfs_manager_ptr->getVFSQuery(query->query, &query->vfs_query)) {
		QEERR_ << "error quetting VFSQuery for " << QUERY_INFO;
	}
	
	
	if(!query_queue.push(query)) {
		QEERR_ << "error submitting for "<< QUERY_INFO;
		delete(query);
	}
	QEDBG_ << "Successfull submission for " << QUERY_INFO;
}

void QueryEngine::process_query() {
	DataCloudQuery *query = NULL;
	while(work_on_query) {
		if(query_queue.pop(query) && query) {
			switch(query->query_phase) {
				case DataCloudQuery::DataCloudQueryPhaseSearch:
					LDBG_ << "Start "<< QUERY_INFO;
					break;

				case DataCloudQuery::DataCloudQueryPhaseAnswer:
					LDBG_ << "Answer "<< QUERY_INFO;
					break;
					
				default:
					//do noting query qil be erased
					break;
			}
			
			if(query->query_phase == DataCloudQuery::DataCloudQueryPhaseError ||
			   query->query_phase == DataCloudQuery::DataCloudQueryPhaseEnd) {
				LDBG_ << "Delete ended "<< QUERY_INFO;
				delete(query);
			} else {
				//reschedule query
				if(!query_queue.push(query)) {
					QEERR_ << "Error rescheduling "<< QUERY_INFO;
				} else {
					QEDBG_ << "Succcesfull rescheduled "<< QUERY_INFO;
				}
			}
		}
		
	}
}