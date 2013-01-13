//
//  main.cpp
//  BSONTestPerf
//
//  Created by Claudio Bisegni on 1/1/13.
//  Copyright (c) 2013 Claudio Bisegni. All rights reserved.
//

#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <chaos/common/bson/bson.h>


int diff(struct timespec* ts_prev, struct timespec* ts){
    return (ts->tv_sec - ts_prev->tv_sec) * 1000 + (ts->tv_nsec - ts_prev->tv_nsec) / 1000000;
}

void current_utc_time(struct timespec *ts) {
    
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, ts);
#endif
    
}

int main(int argc, const char * argv[])
{
    
    // insert code here...
    timespec prevTS = {0,0};
    timespec ts = {0,0};
    int64_t i64 = 20000;
    int64_t i64N = 20000;
    current_utc_time(&prevTS);
    
    for (int i = 0; i < 10000; i++) {
        bson::BSONObjBuilder b;
        b.append("name", "Joe");
        b.append("age", i64);
        bson::BSONObj p = b.obj();
        i64N = p.getField("age").Long();
    }
    current_utc_time(&ts);
    int d = diff(&prevTS, &ts);
    printf("%d.%d\n", prevTS.tv_sec, prevTS.tv_nsec / 1000000);
    printf("%d.%d (%d)\n", ts.tv_sec, ts.tv_nsec / 1000000, d);
    //std::cout << "Time taken is: " << tS.tv_sec << "," << tS.tv_nsec << std::endl;
    return 0;
}

