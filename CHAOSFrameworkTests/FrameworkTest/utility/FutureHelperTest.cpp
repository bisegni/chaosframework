/*
 * Copyright 2012, 22/01/2018 INFN
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
#include "FutureHelperTest.h"
#include <boost/atomic.hpp>
uint32_t future_counter = 0;
uint32_t future_excpt_counter = 0;
uint32_t future_to_counter = 0;
uint32_t promises_counter = 0;
using namespace chaos::common::data;

#define NUMBER_OF_TEST 5000

void CDWComsumerPromise::processBufferElement(PromiseInfo_t *pi, chaos::ElementManagingPolicy& policy) throw(chaos::CException) {
    promises_counter++;
    CDWShrdPtr result(new CDataWrapper());
    result->addInt32Value("pid", pi->promise_id);
    pi->future_helper->setDataForPromiseID(pi->promise_id, result);
}

void CDWComsumerFuture::processBufferElement(FutureInfo_t *fi, chaos::ElementManagingPolicy& policy) throw(chaos::CException) {
    ChaosFutureStatus fret = ChaosFutureStatus::deferred;
    CDWShrdPtr result;
    do{
        ASSERT_NO_THROW(fret = fi->future.wait_for(ChaosCronoMilliseconds(1000)));
        if(fret == ChaosFutureStatus::ready) {
            try{
                result = fi->future.get();
                ASSERT_TRUE(result.get());
                ASSERT_TRUE(result->hasKey("pid"));
                ASSERT_TRUE(result->getInt32Value("pid") == fi->promise_id);
                future_counter++;
            }catch(...){future_excpt_counter++;}
        } else if(fret == ChaosFutureStatus::timeout) {
            future_to_counter++;
        }
    } while(fret == ChaosFutureStatus::deferred);
}

TEST(FutureHelperTests, Base) {
    //boost::thread_group threads;
    CDWComsumerPromise pq;
    CDWComsumerFuture fq;
    MessageRequestDomainFutureHelperShrdPtr helper_test(new MessageRequestDomainFutureHelper());
    MessageRequestDomainFutureHelper::Future        new_shared_future;
    MessageRequestDomainFutureHelper::CounterType   new_id;
    ASSERT_NO_THROW(pq.init(1));
    ASSERT_NO_THROW(fq.init(1));
    ASSERT_NO_THROW(helper_test->init(NULL));
    //generate promise and future
    for (int i = 0; i < NUMBER_OF_TEST; ++i){
        //generate new future
        helper_test->addNewPromise(new_id, new_shared_future);
        ChaosUniquePtr<PromiseInfo> pi(new PromiseInfo(new_id, helper_test));
        ChaosUniquePtr<FutureInfo> fi(new FutureInfo(new_id, new_shared_future));
        while(fq.push(fi.release()) == false);
                    //usleep(100);
        while(pq.push(pi.release()) == false);
    }
    ASSERT_NO_THROW(pq.deinit());
    ASSERT_NO_THROW(fq.deinit());
    ASSERT_EQ(future_excpt_counter, 0);
    ASSERT_EQ(future_to_counter, 0);
    ASSERT_EQ(future_counter, NUMBER_OF_TEST);
    ASSERT_EQ(promises_counter, NUMBER_OF_TEST);
    ASSERT_NO_THROW(helper_test->deinit());
}
