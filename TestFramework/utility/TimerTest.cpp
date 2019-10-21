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

#include <gtest/gtest.h>

#include <chaos/common/utility/Random.h>

#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/random_device.hpp>

#include "TimerTest.h"

using namespace chaos::common::async_central;
uint32_t TimerExample::counter = 0;

void TimerExample::timeout(){
    std::cout << "*";
    counter++;
}

TEST(TimerTest, MultiStartStopReuseSameTimer) {
    chaos::common::utility::Random rnd(1,1000);
    TimerExample t;
    for(int idx = 0; idx < 10; idx++) {
        std::cout << "[          ] " << "-" << idx << "-Start...";
        AsyncCentralManager::getInstance()->addTimer(&t, 0, rnd.rand());
        usleep((unsigned int)(rnd.rand()*1000));
        AsyncCentralManager::getInstance()->removeTimer(&t);
        std::cout << "end" << std::endl;
    }
}

TEST(TimerTest, MultiStartStop) {
    chaos::common::utility::Random rnd(1,1000);
    for(int idx = 0; idx < 10; idx++) {
        TimerExample t;
        std::cout << "[          ] " << "-" << idx << "-Start...";
        AsyncCentralManager::getInstance()->addTimer(&t, 0, rnd.rand());
        usleep((unsigned int)(rnd.rand()*1000));
        AsyncCentralManager::getInstance()->removeTimer(&t);
        std::cout << "end" << std::endl;
    }
}

TEST(TimerTest, MultiStartCounter) {
    TimerExample::counter = 0;
    for(int idx = 0; idx < 10; idx++) {
        TimerExample t;
        std::cout << "[          ] " << "-" << idx << "-Start...";
        AsyncCentralManager::getInstance()->addTimer(&t, 0, 1000);
        usleep(100000);
        AsyncCentralManager::getInstance()->removeTimer(&t);
        std::cout << "end" << std::endl;
    }
    ASSERT_EQ(TimerExample::counter, 10);
}
