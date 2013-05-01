//
//  main.cpp
//  MemcachedEmbeddedTest
//
//  Created by Claudio Bisegni on 3/24/13.
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

#include <chaos/common/global.h>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/data/cache/DataCache.h>
#define STRUCT_NUM 200000


uint64_t diff(struct timespec* ts_prev, struct timespec* ts){
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

typedef struct DemoStruct{
    int32_t a;
    int64_t b[20];
} DemoStruct;

int main(int argc, const char * argv[]) {
    uint32_t faultAllocation = 0;
    timespec prevTS = {0,0};
    timespec ts = {0,0};
    uint32_t bsize;
    void *buff = NULL;
    
    
    
    chaos::memory::ManagedMemory *mm = new chaos::memory::ManagedMemory();
    mm->init(sizeof(DemoStruct), 0, STRUCT_NUM, 0);
    unsigned int sID = mm->getSlabIdBySize(sizeof(DemoStruct));
    
    DemoStruct *structPtr[STRUCT_NUM];
    
    size_t structDim = sizeof(DemoStruct);
    memset(structPtr, 0, sizeof(DemoStruct*)*STRUCT_NUM);
    current_utc_time(&prevTS);
    for (int idx = 0; idx < STRUCT_NUM; idx++) {
        structPtr[idx] = static_cast<DemoStruct*>(mm->allocate(structDim, sID));
        if(structPtr[idx] != NULL) {
            structPtr[idx]->a = idx;
            structPtr[idx]->b[5] = idx*3;
        } else {
            faultAllocation++;
        }
    }
    for (int idx = 0; idx < 1000; idx++) {
        if(structPtr[idx] != NULL) mm->deallocate(structPtr[idx], structDim, sID);
    }
    current_utc_time(&ts);
    uint64_t d = diff(&prevTS, &ts);
    printf("fault allocations: %d\n", faultAllocation);
    printf("%lld.%.9ld\n", (long long)prevTS.tv_sec, prevTS.tv_nsec);
    printf("%lld.%.9ld (%lld)\n", (long long)ts.tv_sec, ts.tv_nsec, d);
    delete mm;
    
    
    //param for cache
    chaos::data::cache::CacheSettings settings;
    
    settings.factor = 1.25;
    settings.maxbytes = 1 * 1024 * 1024; //default is 64MB
    settings.chunk_size = 48; //48;         /* space for a modest key and value */
    settings.item_size_max = 1024 * 1024; //1024 * 1024;
    settings.evict_to_free = 1;
    settings.oldest_live = 0;
    settings.use_cas = 1;
    settings.preallocation = 0;
    auto_ptr<chaos::data::cache::DataCache> fc(new chaos::data::cache::DataCache());
    fc->init(&settings);
    fc->start();
    fc->storeItem("key", "value", (int32_t)strlen("value"));
    fc->getItem("key", bsize, &buff);
    std::cout << (const char *)buff << std::endl;
    
    fc->storeItem("key", "value2", (int32_t)strlen("value2"));
    fc->getItem("key", bsize, &buff);
    std::cout << (const char *)buff  << std::endl;
    
    fc->deleteItem("key");
    fc->storeItem("key", "value3", (int32_t)strlen("value3"));
    fc->getItem("key", bsize, &buff);
    std::cout << (const char *)buff  << std::endl;
    fc->stop();
    fc->deinit();
    
    return 0;
}

