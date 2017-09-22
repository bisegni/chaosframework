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
#include <iostream>
#include <string>

#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/data/cache/KeyGroupCache.h>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;
using namespace std;
#define STRUCT_NUM 10

#define WRITE_THREAD_UPDATE_RATE 2
#define READ_THREAD_NUMBER 10
#define READ_THREAD_UPDATE_RATE_MS_MAX 2
#define GARBAGE_THREAD_UPDATE_RATE_MS 100
#define TEST_DURATION_IN_SEC 5

#define INT32_TEST_VALUE numeric_limits<int32_t>::max()

uint64_t readCount = 0;
uint64_t writeCount = 0;

uint16_t rUpdateMs = READ_THREAD_UPDATE_RATE_MS_MAX;
uint16_t wUpdateMs = WRITE_THREAD_UPDATE_RATE;

bool threadWriteExecution = true;
bool threadReadExecution = true;


void mcedbCacheUpdaterI32(chaos::common::data::cache::DataCache *cPtr) {
    int32_t i32TVal = INT32_TEST_VALUE;
    do{
        cPtr->storeItem("i32k", &i32TVal, sizeof(int32_t));
        writeCount++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(wUpdateMs));
    } while (threadWriteExecution);
}

void mcedbCacheReader(chaos::common::data::cache::DataCache *cPtr) {
    uint32_t dim = 0;
    int32_t val = 0;
    do {
        cPtr->getItem("i32k", dim, &val);
        if(val)assert(val == INT32_TEST_VALUE);
        readCount++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(rUpdateMs));
    } while (threadReadExecution);
}

int main(int argc, const char * argv[]) {
    boost::thread_group tWriterGroup;
    boost::thread_group tGarbageGroup;
    boost::thread_group tReadersGroup;
    po::variables_map vm;
    po::options_description desc("Allowed options");
    
    //test dataset chache
    
    desc.add_options()("help", "Help Message");
    desc.add_options()("n_reader", po::value< uint16_t >()->default_value(READ_THREAD_NUMBER));
    desc.add_options()("r_update_ms", po::value< uint16_t >()->default_value(READ_THREAD_UPDATE_RATE_MS_MAX));
    desc.add_options()("w_update_ms", po::value< uint16_t >()->default_value(WRITE_THREAD_UPDATE_RATE));
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    
    try {
        uint16_t readersNumber = vm["n_reader"].as<uint16_t>();
        rUpdateMs = vm["r_update_ms"].as<uint16_t>();
        wUpdateMs = vm["w_update_ms"].as<uint16_t>();
        
        std::cout << "Start DataCache test (Memcached embedded) with for " << TEST_DURATION_IN_SEC << " seconds " << std::endl;
        std::cout << "Number of writer " << 1 << " at rate of " << wUpdateMs << " ms" << std::endl;
        std::cout << "Number of reader " << readersNumber << " at rate of " << rUpdateMs << " ms" << std::endl;
        
        //test memcached implementation
        chaos::common::data::cache::CacheSettings settings;
        
        settings.factor = 1.25;
        settings.maxbytes = 1 * 1024 * 1024;    // default is 64MB
        settings.chunk_size = 48;               // space for a modest key and value
        settings.item_size_max = 1024 * 1024;   // 1024 * 1024;
        settings.evict_to_free = 1;
        settings.oldest_live = 0;
        settings.use_cas = 0;
        settings.preallocation = 0;
        
        ChaosUniquePtr<chaos::common::data::cache::DataCache> fc(new chaos::common::data::cache::DataCache());
        fc->init(&settings);
        fc->start();
        
        // allocate and start writer thread
        tWriterGroup.create_thread(boost::bind(mcedbCacheUpdaterI32, fc.get()));
        //start all reader thread
        for (int idx = 0; idx < readersNumber; idx++) {
            tReadersGroup.create_thread(boost::bind(mcedbCacheReader, fc.get()));
        }
        
        boost::this_thread::sleep_for(boost::chrono::seconds(TEST_DURATION_IN_SEC));
        threadReadExecution = false;
        std::cout << "Stop all threads" << std::endl;
        //join on read thread
        tReadersGroup.join_all();
        
        //stop writer and garbag thread
        threadWriteExecution = false;
        tWriterGroup.join_all();
        
        std::cout << "Total write operation " << writeCount << std::endl;
        std::cout << "Total read operation " << readCount << std::endl;
        std::cout << "DataCache test ended" << std::endl;
        fc->stop();
        fc->deinit();
    } catch(chaos::CException& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    } catch(...) {
        return 1;
    }
    return 0;
}
