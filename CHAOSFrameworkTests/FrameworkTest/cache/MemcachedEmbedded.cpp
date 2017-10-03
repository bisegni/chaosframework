/*
 * Copyright 2012, 02/10/2017 INFN
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
#include "MemcachedEmbedded.h"
#define STRUCT_NUM 10

#define WRITE_THREAD_UPDATE_RATE 2
#define READ_THREAD_NUMBER 10
#define READ_THREAD_UPDATE_RATE_MS_MAX 2
#define GARBAGE_THREAD_UPDATE_RATE_MS 100
#define TEST_DURATION_IN_SEC 5

#define INT32_TEST_VALUE numeric_limits<int32_t>::max()

void MemcachedEmbeddedTest::SetUp() {
    read_count = 0;
    write_count = 0;
    rUpdateMs = READ_THREAD_UPDATE_RATE_MS_MAX;
    wUpdateMs = WRITE_THREAD_UPDATE_RATE;
    threadWriteExecution = true;
    threadReadExecution = true;
    readersNumber = READ_THREAD_NUMBER;
}

void MemcachedEmbeddedTest::mcedbCacheUpdaterI32(chaos::common::data::cache::DataCache *cPtr) {
    int32_t i32TVal = INT32_TEST_VALUE;
    do{
        ASSERT_NO_THROW(cPtr->storeItem("i32k", &i32TVal, sizeof(int32_t)););
        write_count++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(wUpdateMs));
    } while (threadWriteExecution);
}

void MemcachedEmbeddedTest::mcedbCacheReader(chaos::common::data::cache::DataCache *cPtr) {
    uint32_t dim = 0;
    int32_t val = 0;
    do {
        ASSERT_NO_THROW(cPtr->getItem("i32k", dim, &val););
        if(val){ASSERT_EQ(val, INT32_TEST_VALUE);};
        read_count++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(rUpdateMs));
    } while (threadReadExecution);
}

//TEST_F(MemcachedEmbeddedTest, MemcachedEmbeddedTest) {
//    boost::thread_group tWriterGroup;
//    boost::thread_group tGarbageGroup;
//    boost::thread_group tReadersGroup;
//    
//    
//    //test memcached implementation
//    chaos::common::data::cache::CacheSettings settings;
//    
//    settings.factor = 1.25;
//    settings.maxbytes = 1 * 1024 * 1024;    // default is 64MB
//    settings.chunk_size = 48;               // space for a modest key and value
//    settings.item_size_max = 1024 * 1024;   // 1024 * 1024;
//    settings.evict_to_free = 1;
//    settings.oldest_live = 0;
//    settings.use_cas = 0;
//    settings.preallocation = 0;
//    
//    ChaosUniquePtr<chaos::common::data::cache::DataCache> fc(new chaos::common::data::cache::DataCache());
//    ASSERT_NO_THROW(fc->init(&settings););
//    ASSERT_NO_THROW(fc->start(););
//    
//    // allocate and start writer thread
//    tWriterGroup.create_thread(boost::bind(&MemcachedEmbeddedTest::mcedbCacheUpdaterI32, this, fc.get()));
//    for (int idx = 0; idx < readersNumber; idx++) {
//        tReadersGroup.create_thread(boost::bind(&MemcachedEmbeddedTest::mcedbCacheReader, this, fc.get()));
//    }
//    boost::this_thread::sleep_for(boost::chrono::seconds(TEST_DURATION_IN_SEC));
//    threadReadExecution = false;
//    //join on read thread
//    tReadersGroup.join_all();
//    
//    //stop writer and garbag thread
//    threadWriteExecution = false;
//    tWriterGroup.join_all();
//    ASSERT_NO_THROW(fc->stop(););
//    ASSERT_NO_THROW(fc->deinit(););
//}

