/*
 *	cronous_manager.h
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

#ifndef __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronous_manager_h
#define __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronous_manager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/cronous_manager/CronJob.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace cronous_manager {
            #define CHECK_TIME 10000
            //! define  avector for the running job
            typedef ChaosSharedPtr<boost::thread> ThreadJobShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ThreadJobShrdPtr, MapJobThread);
            
            //!define vector for job instances
            
            //!defin eth eshared ptr for CronJob
            typedef ChaosSharedPtr<cronous_manager::CronJob> JobInstanceShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, JobInstanceShrdPtr, MapJobInstance);
            
            //!Manager class for launch batch job a specific time intervall
            class CronousManager:
            public chaos::common::async_central::TimerHandler,
            public chaos::common::utility::InizializableService {
                uint64_t scheduler_repeat_time;
                //!job tracking structures
                chaos::common::utility::LockableObject<MapJobInstance> map_job_instance;
                chaos::common::utility::LockableObject<MapJobThread> map_job_in_execution;
                
                void clearCompletedJob(bool timed_wait,
                                       unsigned int max_element_to_scan = 0);
            protected:
                //!intherited by @chaos::common::async_central::TimerHandler
                void timeout();
            public:
                CronousManager(uint64_t _scheduler_repeat_time = CHECK_TIME);
                ~CronousManager();
                //!intherited by @chaos::common::utility::InizializableService
                void init(void *init_data) throw (chaos::CException);
                
                //!intherited by @chaos::common::utility::InizializableService
                void deinit() throw (chaos::CException);
                
                //! add a new job to the cron manager
                /*!
                 \return the uinique id of the job
                 */
                virtual bool addJob(CronJob *new_job,
                            std::string& job_index,
                            uint64_t repeat_delay,
                            uint64_t offset = 0);
                //!remove a job
                bool removeJob(const std::string& job_index);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronous_manager_h */
