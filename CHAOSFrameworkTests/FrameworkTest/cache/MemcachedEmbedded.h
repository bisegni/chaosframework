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

#include <gtest/gtest.h>

#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/data/cache/KeyGroupCache.h>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

class MemcachedEmbeddedTest:
public testing::Test {
protected:
    uint64_t read_count;
    uint64_t write_count;
    uint16_t rUpdateMs;
    uint16_t wUpdateMs;
    uint16_t readersNumber;
    bool threadWriteExecution;
    bool threadReadExecution;
public:
    void SetUp();
    void mcedbCacheUpdaterI32(chaos::common::data::cache::DataCache *cPtr);
    void mcedbCacheReader(chaos::common::data::cache::DataCache *cPtr);
};
