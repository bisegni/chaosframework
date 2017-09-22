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

#ifndef __CHAOSFramework__63972CB_F1A7_494B_A797_754F7CAC49AA_CronJob_h
#define __CHAOSFramework__63972CB_F1A7_494B_A797_754F7CAC49AA_CronJob_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataVariant.h>

namespace chaos {
    namespace common {
        namespace cronous_manager {
            
            //!forward declaration
            class CronousManager;
            
            typedef enum CronJobState {
                CronJobStateWaiting,
                CronJobStateRunning
            }CronJobState;
            
            //!define map for job parameter
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::data::CDataVariant, MapKeyVariant);

            
            //! abstraction of a job that can be execution to a specifi intervall of time
            class CronJob {
                friend class CronousManager;
                //!keep track of cron job state
                CronJobState run_state;
                MapKeyVariant job_parameter;
                const std::string job_index;
                
                //!is the timestamp for the next job start
                uint64_t next_ts_start;
                uint64_t repeat_delay;

                //!parse a cdatawrpper to create a parameter map
                void parserCDataWrapperForMapParameter(chaos::common::data::CDataWrapper *param);
            protected:
                //signal the starting of the job
                virtual void start() = 0;
                //!execute the job
                /*!
                 \param job_parameter the parametter of the job
                 \return true if job has finisched, false otherwhise
                 */
                virtual bool execute(const MapKeyVariant& job_parameter) = 0;
                
                //!signal the end of the job
                virtual void end() = 0;
                
                void threadEntry();
                
                void log(const std::string& log_message);
            public:
                CronJob(chaos::common::data::CDataWrapper *job_parameter);
                ~CronJob();
            };
        }
    }
}

#endif /* __CHAOSFramework__63972CB_F1A7_494B_A797_754F7CAC49AA_CronJob_h */
