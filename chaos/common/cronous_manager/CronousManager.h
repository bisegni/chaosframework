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
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/cronous_manager/CronJob.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {

        //! define  avector for the running job
        typedef boost::shared_ptr<boost::thread> ThreadJobShrdPtr;
        CHAOS_DEFINE_VECTOR_FOR_TYPE(ThreadJobShrdPtr, VectorJobThread);
        
        //!define vector for job instances
        typedef boost::shared_ptr<cronous_manager::CronJob> JobInstanceShrdPtr;
        CHAOS_DEFINE_VECTOR_FOR_TYPE(JobInstanceShrdPtr, VectorJobInstance);
        
        namespace cronous_manager {
            
            //!Manager class for launch batch job a specific time intervall
            class CronousManager:
            public chaos::common::async_central::TimerHandler,
            public chaos::common::utility::InizializableService,
            public chaos::common::utility::Singleton<CronousManager> {
                friend class chaos::common::utility::Singleton<CronousManager>;
                
                //!job tracking structures
                chaos::common::utility::LockableObject<VectorJobInstance> vector_job_instance;
                VectorJobThread vector_job_in_execution;
                
                CronousManager();
                ~CronousManager();
                void clearCompletedJob();
                void scanForJobToLaunch();
            protected:
                //!intherited by @chaos::common::async_central::TimerHandler
                void timout();
            public:
                //!intherited by @chaos::common::utility::InizializableService
                void init(void *init_data) throw (chaos::CException);
                
                //!intherited by @chaos::common::utility::InizializableService
                void deinit() throw (chaos::CException);
                
                //! add a new job to the cron manager
                void addJob(CronJob *new_job,
                            uint64_t repeat_delay,
                            uint64_t offset = 0);
                
            };
            
        }
    }
}

#endif /* __CHAOSFramework__C047152_14CC_43E5_8093_CD4B770A75C4_cronous_manager_h */
