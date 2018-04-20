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
#include "EmbeddedLFCacheTest.h"
#define STRUCT_NUM 10

#define WRITE_THREAD_UPDATE_RATE 2
#define READ_THREAD_NUMBER 10
#define READ_THREAD_UPDATE_RATE_MS_MAX 2
#define GARBAGE_THREAD_UPDATE_RATE_MS 100
#define TEST_DURATION_IN_SEC 5

#define INT32_TEST_VALUE numeric_limits<int32_t>::max()
void EmbeddedLFCacheTest::SetUp() {
    read_count = 0;
    write_count = 0;
    rUpdateMs = READ_THREAD_UPDATE_RATE_MS_MAX;
    wUpdateMs = WRITE_THREAD_UPDATE_RATE;
    threadWriteExecution = true;
    threadReadExecution = true;
    readersNumber = READ_THREAD_NUMBER;
    gUpdateMs = GARBAGE_THREAD_UPDATE_RATE_MS;
}
void EmbeddedLFCacheTest::cacheUpdaterI32(::chaos::common::data::cache::KeyGroupCache *cPtr) {
    do{
        int32_t i32TVal = INT32_TEST_VALUE;
        cPtr->updateKeyValue((uint16_t)0, &i32TVal);
        write_count++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(wUpdateMs));
    } while (threadWriteExecution);
}

void EmbeddedLFCacheTest::cacheReader(chaos::common::data::cache::KeyGroupCache *cPtr) {
    chaos::common::data::cache::ChannelValueAccessor accessor;
    do {
        cPtr->getCurrentKeyAccessor((uint16_t)0, accessor);
        int32_t readed = *accessor.getValuePtr<int32_t>();
        if(readed) {
            ASSERT_EQ(readed, INT32_TEST_VALUE);
            read_count++;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(rUpdateMs));
    } while (threadReadExecution);
}


void EmbeddedLFCacheTest::cacheGarbage(chaos::common::data::cache::KeyGroupCache *cPtr) {
    do {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(gUpdateMs));
        cPtr->garbageCache();
    } while (threadWriteExecution);
}

TEST_F(EmbeddedLFCacheTest, EmbeddedLFCacheTest) {
//    boost::thread_group tWriterGroup;
//    boost::thread_group tGarbageGroup;
//    boost::thread_group tReadersGroup;
//
//    ChaosUniquePtr<chaos::common::data::cache::KeyGroupCache> dsCache(new chaos::common::data::cache::KeyGroupCache());
//    dsCache->addKeyInfo("ch_i32", chaos::DataType::TYPE_INT32);
//    ASSERT_NO_THROW(dsCache->init(NULL););
//    ASSERT_NO_THROW(dsCache->start(););
//    
//    tWriterGroup.create_thread(boost::bind(&EmbeddedLFCacheTest::cacheUpdaterI32, this, dsCache.get()));
//    tGarbageGroup.create_thread(boost::bind(&EmbeddedLFCacheTest::cacheGarbage, this, dsCache.get()));
//    for (int idx = 0; idx < readersNumber; idx++) {
//        tReadersGroup.create_thread(boost::bind(&EmbeddedLFCacheTest::cacheReader, this, dsCache.get()));
//    }
//
//    boost::this_thread::sleep_for(boost::chrono::seconds(TEST_DURATION_IN_SEC));
//    threadReadExecution = false;
//    //join on read thread
//    tReadersGroup.join_all();
//
//    //stop writer and garbag thread
//    threadWriteExecution = false;
//    tGarbageGroup.join_all();
//
//    tWriterGroup.join_all();
//
//    //deinit all cache
//    ASSERT_NO_THROW(dsCache->stop(););
//    ASSERT_NO_THROW(dsCache->deinit(););
}


