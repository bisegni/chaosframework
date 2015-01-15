//
//  main.cpp
//  MemcachedEmbeddedTest
//
//  Created by Claudio Bisegni on 3/24/13.
//  Copyright (c) 2013 Claudio Bisegni. All rights reserved.
//
#include <iostream>
#include <string>

#include <chaos/common/memory/ManagedMemory.h>
#include <chaos/common/data/cache/DataCache.h>
#include <chaos/common/data/cache/KeyGroupCache.h>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
namespace po = boost::program_options;
using namespace std;
#define WRITE_THREAD_UPDATE_RATE 2
#define READ_THREAD_NUMBER 1
#define READ_THREAD_UPDATE_RATE_MS_MAX 2
#define GARBAGE_THREAD_UPDATE_RATE_MS 100
#define TEST_DURATION_IN_SEC 10

#define INT32_TEST_VALUE numeric_limits<int32_t>::max()

uint64_t readCount = 0;
uint64_t writeCount = 0;

uint16_t rUpdateMs = READ_THREAD_UPDATE_RATE_MS_MAX;
uint16_t wUpdateMs = WRITE_THREAD_UPDATE_RATE;
uint16_t gUpdateMs = GARBAGE_THREAD_UPDATE_RATE_MS;

bool threadWriteExecution = true;
bool threadReadExecution = true;

void cacheUpdaterI32(::chaos::common::data::cache::KeyGroupCache *cPtr) {
    do{
        int32_t i32TVal = INT32_TEST_VALUE;
        cPtr->updateKeyValue((uint16_t)0, &i32TVal);
        writeCount++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(wUpdateMs));
    } while (threadWriteExecution);
}

void cacheReader(chaos::common::data::cache::KeyGroupCache *cPtr) {
    chaos::common::data::cache::ChannelValueAccessor accessor;
    do {
        cPtr->getCurrentKeyAccessor((uint16_t)0, accessor);
        int32_t readed = *accessor.getValuePtr<int32_t>();
        if(readed) {
            if(readed != INT32_TEST_VALUE) {
                std::cout << "wrong value readed " << readed << std::endl;
            }
        } else {
            std::cout << "value not read" << readed << std::endl;
        }
        readCount++;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(rUpdateMs));
    } while (threadReadExecution);
}


void cacheGarbage(chaos::common::data::cache::KeyGroupCache *cPtr) {
    do {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(gUpdateMs));
        cPtr->garbageCache();
    } while (threadWriteExecution);
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
    desc.add_options()("g_update_ms", po::value< uint16_t >()->default_value(GARBAGE_THREAD_UPDATE_RATE_MS));
    
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    
    try {
        boost::timer::auto_cpu_timer cpuTimer;
        uint16_t readersNumber = vm["n_reader"].as<uint16_t>();
        rUpdateMs = vm["r_update_ms"].as<uint16_t>();
        wUpdateMs = vm["w_update_ms"].as<uint16_t>();
        gUpdateMs = vm["g_update_ms"].as<uint16_t>();
        
        std::cout << "Start DataCache test (Memcached embedded) with for " << TEST_DURATION_IN_SEC << " seconds " << std::endl;
        std::cout << "Number of writer " << 1 << " at rate of " << wUpdateMs << " ms" << std::endl;
        std::cout << "Number of reader " << readersNumber << " at rate of " << rUpdateMs << " ms" << std::endl;
        std::cout << "Garbager at rate of " << gUpdateMs << " ms" << std::endl;
        
	auto_ptr<chaos::common::data::cache::KeyGroupCache> dsCache(new chaos::common::data::cache::KeyGroupCache());
        dsCache->addKeyInfo("ch_i32", chaos::DataType::TYPE_INT32);
        dsCache->init(NULL);
        dsCache->start();
        
        tWriterGroup.create_thread(boost::bind(cacheUpdaterI32, dsCache.get()));
        //start garbage thread
        tGarbageGroup.create_thread(boost::bind(cacheGarbage, dsCache.get()));
        //start all reader thread
        for (int idx = 0; idx < readersNumber; idx++) {
            tReadersGroup.create_thread(boost::bind(cacheReader, dsCache.get()));
        }
        
        boost::this_thread::sleep_for(boost::chrono::seconds(TEST_DURATION_IN_SEC));
        threadReadExecution = false;
        std::cout << "Stop all threads" << std::endl;
        //join on read thread
        tReadersGroup.join_all();
        
        //stop writer and garbag thread
        threadWriteExecution = false;
        tGarbageGroup.join_all();
        
        tWriterGroup.join_all();
        
        //deinit all cache
        dsCache->stop();
        dsCache->deinit();
        std::cout << "Total write operation " << writeCount << std::endl;
        std::cout << "Total read operation " << readCount << std::endl;
        std::cout << "Thread stopped and KeyGroupCache deinitialized" << std::endl;
    } catch(chaos::CException& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    } catch(...) {
        return 1;
    }
    return 0;
}

