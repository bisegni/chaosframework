/*
 *	cronous_manager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/09/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/cronous_manager/CronousManager.h>

using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::cronous_manager;

#define CHECK_TIME 10000
#define CHECK_THREAD_MAX_ELEMENT 5
#define MAX_NUMBER_OF_CONCURRENT_EXECUTION 10

#pragma mark Public Methods
CronousManager::CronousManager(){
    
}

CronousManager::~CronousManager() {
    
}

void CronousManager::init(void *init_data) throw (chaos::CException) {
    //register as timer
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 CHECK_TIME,
                                                 CHECK_TIME);
}

void CronousManager::deinit() throw (chaos::CException) {
    //register as timer
    AsyncCentralManager::getInstance()->removeTimer(this);
    
    //! wait thermitation of all jobs
    clearCompletedJob(false);
}

bool CronousManager::addJob(CronJob *new_job,
                            std::string& job_index,
                            uint64_t repeat_delay,
                            uint64_t offset) {
    if(new_job == NULL) return false;
    
    LockableObjectWriteLock wl;
    map_job_instance.getWriteLock(wl);
    new_job->repeat_delay = repeat_delay;
    new_job->next_ts_start = (TimingUtil::getTimeStamp()+offset);
    map_job_instance().insert(MapJobInstancePair(new_job->job_index,
                                                 JobInstanceShrdPtr(new_job)));
    job_index = new_job->job_index;
    return true;
}

bool CronousManager::removeJob(const std::string& job_index) {
    LockableObjectWriteLock wl;
    map_job_in_execution.getWriteLock(wl);
    
    if(map_job_in_execution().count(job_index) == 0) return false;
    
    map_job_in_execution()[job_index]->interrupt();
    map_job_in_execution()[job_index]->join();
    map_job_in_execution().erase(job_index);
    
    //remove the instance
    LockableObjectWriteLock wl_instance;
    map_job_instance().erase(job_index);
    return true;
}

#pragma mark Protected Methods
void CronousManager::timeout() {
    //scan job to start
    uint64_t current_ts = TimingUtil::getTimeStamp();
    
    LockableObjectReadLock wl_on_instance;
    map_job_instance.getReadLock(wl_on_instance);
    
    //scann instance to determinate wich one need to be started
    if(map_job_in_execution().size()<MAX_NUMBER_OF_CONCURRENT_EXECUTION) {
        //lock the vecto rof instance
        
        
        //scans instance to find wich one need to be started
        for(MapJobInstanceIterator it = map_job_instance().begin(),
            end = map_job_instance().end();
            it != end;
            it++) {
            if(it->second->run_state == CronJobStateWaiting &&
               current_ts >= (*it->second).next_ts_start) {
                (*it->second).run_state = CronJobStateRunning;
                
                //start the execution of the job into a thread
                LockableObjectWriteLock wl_thread;
                map_job_in_execution.getWriteLock(wl_thread);
                map_job_in_execution().insert(MapJobThreadPair(it->first,
                                                               ThreadJobShrdPtr(new boost::thread(boost::bind(&CronJob::threadEntry,
                                                                                                              it->second.get())))));
                //check if we have reached the maximum number fo concurrent jobs
                if(map_job_in_execution().size()>=MAX_NUMBER_OF_CONCURRENT_EXECUTION){break;}
            }
        }
    }
    
    clearCompletedJob(true,
                      CHECK_THREAD_MAX_ELEMENT);
}

#pragma mark Private Methods

void CronousManager::clearCompletedJob(bool timed_wait,
                                       unsigned int max_element_to_scan) {
    //check started job to determinate which has finisched
    unsigned int element_count = 0;
    uint64_t current_ts = TimingUtil::getTimeStamp();
    
    LockableObjectWriteLock wl_thread;
    map_job_in_execution.getWriteLock(wl_thread);
    MapJobThreadIterator it = map_job_in_execution().begin();
    MapJobThreadIterator end = map_job_in_execution().end();
    while(it != end) {
        if((*it->second).joinable()){
            if((*it->second).try_join_for(boost::chrono::milliseconds(10))){
                map_job_in_execution().erase(it);
            } 
	    it++;
        }
        //stop in case we have processed the maximum number of thread
        if(max_element_to_scan &&
           element_count++>=max_element_to_scan){break;}
    }
}


