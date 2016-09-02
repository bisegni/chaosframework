/*
 *	DeviceDataWorker.cpp
 *	!CHAOS
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

#include "../ChaosDataService.h"
#include "DeviceSharedDataWorker.h"
#include "../cache_system/CacheDriverMetricCollector.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>
using namespace chaos::common::utility;
using namespace chaos::data_service::worker;
namespace chaos_vfs = chaos::data_service::vfs;

#define DSDW_LAPP_ INFO_LOG(DeviceSharedDataWorker)
#define DSDW_LDBG_ DBG_LOG(DeviceSharedDataWorker)
#define DSDW_LERR_ ERR_LOG(DeviceSharedDataWorker)


//------------------------------------------------------------------------------------------------------------------------

DeviceSharedDataWorker::DeviceSharedDataWorker(const std::string& _cache_impl_name,
                                               const std::string& _object_impl_name):
cache_impl_name(_cache_impl_name),
object_impl_name(_object_impl_name){}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
    DataWorker::init(init_data);
    
    DSDW_LAPP_ << "allocating cache driver for every thread";
    
    for(int idx = 0; idx < settings.job_thread_number; idx++) {
        ThreadCookie *_tc_ptr = new ThreadCookie();
        _tc_ptr->persistence_driver = NULL;
        _tc_ptr->obj_storage_da = NULL;
        //associate the thread cooky for the idx value
        thread_cookie[idx] = _tc_ptr;
    }
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
    
    for(int idx = 0; idx < settings.job_thread_number; idx++) {
        ThreadCookie *tmp_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
        
    }
    
    std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
    DataWorker::deinit();
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
    int err = 0;
    DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
    ThreadCookie *this_thread_cookie = reinterpret_cast<ThreadCookie *>(cookie);
    
    CHAOS_ASSERT(job_ptr)
    CHAOS_ASSERT(job_ptr->request_header);
    CHAOS_ASSERT(job_ptr->data_pack);
    CHAOS_ASSERT(this_thread_cookie)
    //CHAOS_ASSERT(this_thread_cookie->vfs_stage_file)
    
    //check what kind of push we have
    //read lock on mantainance mutex
    switch(job_ptr->request_header->tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //write data on object storage
            
            free(job_ptr->request_header);
            free(job_ptr->data_pack);
            free(job_ptr);
            break;
            
        case 1:{// live only
            break;
        }
    }
}

//!
void DeviceSharedDataWorker::mantain() throw (chaos::CException) {
    // lock for mantains
    for(int idx = 0; idx < settings.job_thread_number; idx++) {
        //ThreadCookie *current_tread_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
        // CHAOS_ASSERT(current_tread_cookie)
        //write lock on mantainance mutex
        //boost::unique_lock<boost::shared_mutex> rl(current_tread_cookie->mantainance_mutex);
        
        //mantainance on virtual file
        //current_tread_cookie->vfs_stage_file->mantain();
    }
}
