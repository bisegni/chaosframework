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

#ifndef __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronus_manager_h
#define __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronus_manager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/cronus_manager/CronJob.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace cronus_manager {
            #define CHECK_TIME 10000
            //! define  avector for the running job
            typedef ChaosSharedPtr<boost::thread> ThreadJobShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ThreadJobShrdPtr, MapJobThread);
            
            //!define vector for job instances
            
            //!defin eth eshared ptr for CronJob
            typedef ChaosSharedPtr<cronus_manager::CronJob> JobInstanceShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, JobInstanceShrdPtr, MapJobInstance);
            
            //!Manager class for launch batch job a specific time intervall
            class CronusManager:
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
                CronusManager(uint64_t _scheduler_repeat_time = CHECK_TIME);
                ~CronusManager();
                //!intherited by @chaos::common::utility::InizializableService
                void init(void *init_data) ;
                
                //!intherited by @chaos::common::utility::InizializableService
                void deinit() ;
                
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

#endif /* __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronus_manager_h */
