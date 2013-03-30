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

typedef struct DemoStruct{
    int32_t a;
    int64_t b[20];
} DemoStruct;

int main(int argc, const char * argv[])
{
    int32_t bsize;
    void *buff = NULL;
    
    
    
    chaos::memory::ManagedMemory *mm = new chaos::memory::ManagedMemory(48, 1*1024*1024, 4*1024*1024, 1.12, 0);
    mm->init();
    unsigned int sID = mm->getSlabIdBySize(sizeof(DemoStruct));
    
    DemoStruct *structPtr[1000];
    for (int idx = 0; idx < 1000; idx++) {
        structPtr[idx] = static_cast<DemoStruct*>(mm->allocate(sizeof(DemoStruct), sID));
        structPtr[idx]->a = idx;
        structPtr[idx]->b[5] = idx*3;
    }
    for (int idx = 0; idx < 1000; idx++) {
        mm->deallocate(structPtr[idx], sizeof(DemoStruct), sID);
    }

    delete mm;
    
    chaos::datacache::CacheInitParameter memCacheSettings;
    memCacheSettings.factor = 1.25;
    memCacheSettings.maxByte = 1 * 1024 * 1024; //default is 64MB
    memCacheSettings.chunkSize = 48; //48;         /* space for a modest key and value */
    memCacheSettings.maxItemByte = 1024 * 1024; //1024 * 1024;
  
    chaos::datacache::DataCache::getInstance()->init(&memCacheSettings);
    
    chaos::datacache::DataCache::getInstance()->start();
    
    chaos::datacache::DataCache::getInstance()->storeItem("key", "value", (int32_t)strlen("value"));
    
    chaos::datacache::DataCache::getInstance()->getItem("key", bsize, &buff);
    
    std::cout << (const char *)buff << std::endl;
    
    chaos::datacache::DataCache::getInstance()->storeItem("key", "value2", (int32_t)strlen("value2"));
    
    chaos::datacache::DataCache::getInstance()->getItem("key", bsize, &buff);
    
    std::cout << (const char *)buff  << std::endl;
    
    chaos::datacache::DataCache::getInstance()->deleteItem("key");
    
    chaos::datacache::DataCache::getInstance()->storeItem("key", "value2", (int32_t)strlen("value2"));
    
    chaos::datacache::DataCache::getInstance()->getItem("key", bsize, &buff);
    
    std::cout << (const char *)buff  << std::endl;
    
    chaos::datacache::DataCache::getInstance()->stop();
    
    chaos::datacache::DataCache::getInstance()->deinit();
    return 0;
}

