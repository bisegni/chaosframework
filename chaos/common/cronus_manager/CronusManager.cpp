/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/cronus_manager/CronusManager.h>

using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::cronus_manager;


#define CHECK_THREAD_MAX_ELEMENT 5
#define MAX_NUMBER_OF_CONCURRENT_EXECUTION 10

#pragma mark Public Methods
CronusManager::CronusManager(uint64_t _scheduler_repeat_time):
scheduler_repeat_time(_scheduler_repeat_time){}

CronusManager::~CronusManager() {}

void CronusManager::init(void *init_data)  {
    //register as timer
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 scheduler_repeat_time,
                                                 scheduler_repeat_time);
}

void CronusManager::deinit()  {
    //register as timer
    AsyncCentralManager::getInstance()->removeTimer(this);
    
    //! wait thermitation of all jobs
    clearCompletedJob(false);
}

bool CronusManager::addJob(CronJob *new_job,
                            std::string& job_index,
                            uint64_t repeat_delay,
                            uint64_t offset) {
    if(new_job == NULL) return false;
    
    LockableObjectWriteLock_t wl;
    map_job_instance.getWriteLock(wl);
    new_job->repeat_delay = repeat_delay;
    new_job->next_ts_start = (TimingUtil::getTimeStamp()+offset);
    map_job_instance().insert(MapJobInstancePair(new_job->job_index,
                                                 JobInstanceShrdPtr(new_job)));
    job_index = new_job->job_index;
    return true;
}

bool CronusManager::removeJob(const std::string& job_index) {
    LockableObjectWriteLock_t wl;
    map_job_in_execution.getWriteLock(wl);
    
    if(map_job_in_execution().count(job_index) == 0) return false;
    
    map_job_in_execution()[job_index]->interrupt();
    map_job_in_execution()[job_index]->join();
    map_job_in_execution().erase(job_index);
    
    //remove the instance
    LockableObjectWriteLock_t wl_instance;
    map_job_instance().erase(job_index);
    return true;
}

#pragma mark Protected Methods
void CronusManager::timeout() {
    //scan job to start
    uint64_t current_ts = TimingUtil::getTimeStamp();
    
    LockableObjectReadLock_t wl_on_instance;
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
                LockableObjectWriteLock_t wl_thread;
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

void CronusManager::clearCompletedJob(bool timed_wait,
                                       unsigned int max_element_to_scan) {
    //check started job to determinate which has finisched
    unsigned int element_count = 0;
    
    LockableObjectWriteLock_t wl_thread;
    map_job_in_execution.getWriteLock(wl_thread);
    MapJobThreadIterator it = map_job_in_execution().begin();
    MapJobThreadIterator end = map_job_in_execution().end();
    while(it != end) {
        //incremente and get current
        MapJobThreadIterator current = it++;
        if((*current->second).joinable() &&
           (*current->second).try_join_for(boost::chrono::milliseconds(10))){
            map_job_in_execution().erase(current);
        }
        //stop in case we have processed the maximum number of thread
        if(max_element_to_scan &&
           element_count++>=max_element_to_scan){break;}
    }
}


