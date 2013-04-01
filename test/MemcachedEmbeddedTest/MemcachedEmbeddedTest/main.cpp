//
//  main.cpp
//  MemcachedEmbeddedTest
//
//  Created by Claudio Bisegni on 3/24/13.
//  Copyright (c) 2013 Claudio Bisegni. All rights reserved.
//
#include <iostream>
#include <chaos/common/global.h>
#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/data/cache/DataCache.h>
#define STRUCT_NUM 1000

typedef struct DemoStruct{
    int32_t a;
    int64_t b[20];
} DemoStruct;

int main(int argc, const char * argv[])
{
    int32_t bsize;
    void *buff = NULL;
    
    
    
    chaos::memory::ManagedMemory *mm = new chaos::memory::ManagedMemory();
    mm->init(168, 512*1024, 0, 1.12, 0);
    unsigned int sID = mm->getSlabIdBySize(sizeof(DemoStruct));
    
    DemoStruct *structPtr[STRUCT_NUM];
    
    size_t structDim = sizeof(DemoStruct);
    memset(structPtr, 0, sizeof(DemoStruct*)*STRUCT_NUM);
    
    for (int idx = 0; idx < 1000; idx++) {
        structPtr[idx] = static_cast<DemoStruct*>(mm->allocate(structDim, sID));
        if(structPtr[idx] != NULL) {
            structPtr[idx]->a = idx;
            structPtr[idx]->b[5] = idx*3;
        } else {
            //not enougth memory in slabs class
        }
    }
    for (int idx = 0; idx < 1000; idx++) {
        if(structPtr[idx] != NULL) mm->deallocate(structPtr[idx], structDim, sID);
    }

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

