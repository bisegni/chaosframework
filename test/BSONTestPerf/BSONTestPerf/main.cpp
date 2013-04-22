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
#include <string>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <chaos/common/bson/bson.h>
#include <chaos/common/data/CDataWrapper.h>

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
    
    bson::BSONObjBuilder bFragment(true, 512);
    // insert code here...
    timespec prevTS = {0,0};
    timespec ts = {0,0};
    int64_t i64Max,i64Min = 0;
    uint64_t ui64Max, ui64Min = 0;
    int32_t i32Max, i32Min = 0;
    uint32_t ui32Max, ui32Min = 0;
    std::string strvalue;
    
    bFragment.append("ui64_max", static_cast<int64_t>(ui64Max = std::numeric_limits<uint64_t>::max()));

    
    current_utc_time(&prevTS);
    
    // for (int i = 0; i < 10000; i++) {
    //test BSON
    bson::BSONObjBuilder b;
    b.append("string_key", "string value");
    b.append("i64_max", i64Max = std::numeric_limits<int64_t>::max());
    b.append("i64_min", i64Min = std::numeric_limits<int64_t>::min());
    b.append("ui64_max", static_cast<int64_t>(ui64Max = std::numeric_limits<uint64_t>::max()));
    b.append("ui64_min", static_cast<int64_t>(ui64Min = std::numeric_limits<uint64_t>::min()));
    b.append("i32_max", i32Max = std::numeric_limits<int32_t>::max());
    b.append("i32_min", i32Min = std::numeric_limits<int32_t>::min());
    b.append("ui32_max", static_cast<int32_t>(ui32Max = std::numeric_limits<uint32_t>::max()));
    b.append("ui32_min", static_cast<int32_t>(ui32Min = std::numeric_limits<uint32_t>::min()));
    bson::BSONObj p = b.obj();
    std::cout << "string_key=" << (strvalue = p.getField("string_key").String()) << std::endl;
    std::cout << "i64_max="  << (i64Max = p.getField("i64_max").numberLong()) << std::endl;
    std::cout << "i64_min="  << (i64Min = p.getField("i64_min").numberLong()) << std::endl;
    std::cout << "ui64_max="  << (ui64Max = static_cast<uint64_t>(p.getField("ui64_max").numberLong())) << std::endl;
    std::cout << "ui64_min="  << (ui64Min = static_cast<uint64_t>(p.getField("ui64_min").numberLong())) << std::endl;
    std::cout << "i32_max="  << (i32Max = p.getField("i32_max").numberInt()) << std::endl;
    std::cout << "i32_min="  << (i32Min = p.getField("i32_min").numberInt()) << std::endl;
    std::cout << "ui32_max="  << (ui32Max = static_cast<uint32_t>(p.getField("ui32_max").numberInt())) << std::endl;
    std::cout << "ui32_min="  << (ui32Min = static_cast<uint32_t>(p.getField("ui32_min").numberInt())) << std::endl;
    // }
    current_utc_time(&ts);
    int d = diff(&prevTS, &ts);
    printf("%ld.%ld\n", prevTS.tv_sec, prevTS.tv_nsec / 1000000);
    printf("%ld.%ld (%d)\n", ts.tv_sec, ts.tv_nsec / 1000000, d);
    current_utc_time(&prevTS);
    //test CDataWrapper
    
    chaos::CDataWrapper cd;
    cd.addStringValue("string_key", "string value");
    cd.addInt64Value("i64_max", i64Max);
    cd.addInt64Value("i64_min", i64Min);
    cd.addInt64Value("ui64_max", ui64Max);
    cd.addInt64Value("ui64_min", ui64Min);
    cd.addInt32Value("i32_max", i32Max);
    cd.addInt32Value("i32_min", i32Min);
    cd.addInt32Value("ui32_max", ui32Max);
    cd.addInt32Value("ui32_min", ui32Min);
    std::cout << "string_key=" << cd.getCStringValue("string_key") << std::endl;
    std::cout << "i64_max="  << (i64Max = cd.getInt64Value("i64_max")) << std::endl;
    std::cout << "i64_min="  << (i64Min = cd.getInt64Value("i64_min")) << std::endl;
    std::cout << "ui64_max="  << (ui64Max = cd.getUInt64Value("ui64_max")) << std::endl;
    std::cout << "ui64_min="  << (ui64Min = cd.getUInt64Value("ui64_min")) << std::endl;
    std::cout << "i32_max="  << (i32Max = cd.getInt32Value("i32_max")) << std::endl;
    std::cout << "i32_min="  << (i32Min = cd.getInt32Value("i32_min")) << std::endl;
    std::cout << "ui32_max="  << (ui32Max = cd.getUInt32Value("ui32_max")) << std::endl;
    std::cout << "ui32_min="  << (ui32Min = cd.getUInt32Value("ui32_min")) << std::endl;

    current_utc_time(&ts);
    d = diff(&prevTS, &ts);
    printf("%d.%d\n", prevTS.tv_sec, prevTS.tv_nsec / 1000000);
    printf("%d.%d (%d)\n", ts.tv_sec, ts.tv_nsec / 1000000, d);
    return 0;
}

