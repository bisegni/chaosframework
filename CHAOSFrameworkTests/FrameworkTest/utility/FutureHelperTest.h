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


#include <gtest/gtest.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/thread/FutureHelper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>


typedef chaos::common::thread::FutureHelper<chaos::common::data::CDWShrdPtr> MessageRequestDomainFutureHelper;
typedef ChaosSharedPtr<MessageRequestDomainFutureHelper> MessageRequestDomainFutureHelperShrdPtr;

typedef struct PromiseInfo {
    MessageRequestDomainFutureHelperShrdPtr future_helper;
    MessageRequestDomainFutureHelper::CounterType promise_id;
    PromiseInfo(const MessageRequestDomainFutureHelper::CounterType& _promise_id,
                const MessageRequestDomainFutureHelperShrdPtr& _future_helper):
    promise_id(_promise_id),
    future_helper(_future_helper){}
} PromiseInfo_t;

typedef struct FutureInfo {
    MessageRequestDomainFutureHelper::CounterType promise_id;
    MessageRequestDomainFutureHelper::Future future;
    FutureInfo(const MessageRequestDomainFutureHelper::CounterType& _promise_id,
               const MessageRequestDomainFutureHelper::Future& _future):
    promise_id(_promise_id),
    future(_future){}
} FutureInfo_t;

typedef ChaosSharedPtr<FutureInfo> FutureInfoShrdPtr;

class CDWComsumerPromise:
public chaos::CObjectProcessingQueue<PromiseInfo> {
    void processBufferElement(ChaosUniquePtr<PromiseInfo_t> promises_info) throw(chaos::CException);
};

class CDWComsumerFuture:
public chaos::CObjectProcessingQueue<FutureInfo> {
    void processBufferElement(ChaosUniquePtr<FutureInfo_t> future_info) throw(chaos::CException);
};
