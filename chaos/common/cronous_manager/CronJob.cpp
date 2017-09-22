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

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/cronous_manager/CronJob.h>

using namespace chaos::common::cronous_manager;

#define ILOG INFO_LOG(CronJob);
#define DBG  DBG_LOG(CronJob);
#define ERR  ERR_LOG(CronJob);

using namespace chaos::common::utility;

#pragma mark Public Methods
CronJob::CronJob(chaos::common::data::CDataWrapper *job_parameter):
run_state(CronJobStateWaiting),
next_ts_start(0),
job_index(UUIDUtil::generateUUIDLite()) {
    //inizilize job map
    parserCDataWrapperForMapParameter(job_parameter);
}

CronJob::~CronJob() {}

#pragma mark Protected Methos
void CronJob::threadEntry() {
    INFO_LOG(CronJob) << CHAOS_FORMAT("Entry thread for job %1%", %job_index);
    try{
        start();
        do{
            //check for interruption point
            boost::this_thread::interruption_point();
        }while(execute(job_parameter));
    } catch (boost::thread_interrupted& ex) {
        ERR_LOG(CronJob) << CHAOS_FORMAT("Job %1% has been interrupted", %job_index);
    } catch(...) {
        ERR_LOG(CronJob) << CHAOS_FORMAT("Job %1% has been interrupted", %job_index);
    }
    try{
        end();
    }catch(...){}
    //reset running state
    next_ts_start = TimingUtil::getTimeStamp()+repeat_delay;
    run_state = CronJobStateWaiting;
    INFO_LOG(CronJob) << CHAOS_FORMAT("Leaving thread for job %1%", %job_index);
}

void CronJob::log(const std::string& log_message) {
   INFO_LOG(CronJob) << CHAOS_FORMAT("[%1%] %2%", %job_index%log_message);
}

#pragma mark Private Methos
void CronJob::parserCDataWrapperForMapParameter(chaos::common::data::CDataWrapper *param) {
    if(param == NULL) return;
    job_parameter.clear();
    
    ChaosStringVector keys;
    param->getAllKey(keys);
    
    //scann all keys and create map for input parameter
    for(ChaosStringVectorIterator it = keys.begin(),
        end = keys.end();
        it != end;
        it++) {
        //insert new parameter
        job_parameter.insert(MapKeyVariantPair(*it, param->getVariantValue(*it)));
    }
}
