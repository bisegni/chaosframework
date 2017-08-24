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

#include <chaos/common/utility/ProcStat.h>


using namespace chaos::common::utility;

ProcStat::ProcStat():
usr_time(0.0),
sys_time(0.0),
swap_rsrc(0.0),
uptime(0),
last_usr_time(0.0),
last_sys_time(0.0),
last_total_time(0.0),
last_sampling_time(0),
creation_time(TimingUtil::getTimeStamp()/1000){}

ProcStat& ProcStat::operator=(ProcStat const &rhs) {
    usr_time = rhs.usr_time;
    sys_time = rhs.sys_time;
    swap_rsrc = rhs.swap_rsrc;
    return *this;
}

#define COMPUTE_RESOURCE(m,u) (double)m + ((double)u / 1000000.0)

void ProcStatCalculator::update(ProcStat& stat) {
    //used to compute the resource occupaiton between sampling time
    struct rusage process_resurce_usage;
    struct rusage children_resurce_usage;
    double local_usr_time = 0;
    double local_sys_time = 0;
    //samplig time in seconds
    uint64_t sampling_time = TimingUtil::getTimeStamp()/1000;
    getrusage(RUSAGE_SELF, &process_resurce_usage);
    getrusage(RUSAGE_CHILDREN, &children_resurce_usage);
    local_usr_time = COMPUTE_RESOURCE(process_resurce_usage.ru_utime.tv_sec,
                                      process_resurce_usage.ru_utime.tv_usec);
    local_usr_time += COMPUTE_RESOURCE(children_resurce_usage.ru_utime.tv_sec,
                                       children_resurce_usage.ru_utime.tv_usec);
    local_sys_time = COMPUTE_RESOURCE(process_resurce_usage.ru_stime.tv_sec,
                                      process_resurce_usage.ru_stime.tv_usec);
    local_sys_time += COMPUTE_RESOURCE(children_resurce_usage.ru_stime.tv_sec,
                                       children_resurce_usage.ru_stime.tv_usec);
    if(stat.last_usr_time &&
       stat.last_sys_time &&
       stat.last_sampling_time) {
        uint64_t temp_ts = (sampling_time-stat.last_sampling_time);
        if(temp_ts > 0) {
            stat.usr_time = 100*(local_usr_time-stat.last_usr_time)/(double)temp_ts;
            stat.sys_time = 100*(local_sys_time-stat.last_sys_time)/(double)temp_ts;
        }else {
            stat.usr_time = stat.sys_time = 0;
        }
        
    }
    stat.last_usr_time = local_usr_time;
    stat.last_sys_time = local_sys_time;
    stat.last_sampling_time = sampling_time;
    stat.swap_rsrc = process_resurce_usage.ru_nswap;
    
    stat.uptime = (stat.last_sampling_time-stat.creation_time);
}
