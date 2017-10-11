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

#ifndef chaos_common_utility_PromisesHelper_h
#define chaos_common_utility_PromisesHelper_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/async_central/async_central.h>

#include <chaos/common/utility/InitializableService.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define TIMEOUT_PURGE_PROMISE 60000000
#define PURGE_TIMER_REPEAT_DELAY 30000

namespace chaos{
    namespace common {
        namespace thread {
            
            template<typename T>
            class FutureHelper:
            public chaos::common::async_central::TimerHandler {
            public:
                //!define the types
                typedef ChaosPromise< T >   Promise;
                typedef ChaosFuture< T >    Future;
            private:
                struct PromisesInfo {
                    uint32_t promise_id;
                    int64_t timeout_ts;
                    Promise promise;
                    typedef ChaosSharedPtr< PromisesInfo > PromisesInfoShrdPtr;
                    //!key accessors for multindix infrastructure
                    struct extract_index {
                        typedef uint32_t result_type;
                        const result_type &operator()(const PromisesInfoShrdPtr &p) const;
                    };
                    
                    struct extract_req_ts {
                        typedef int64_t result_type;
                        const result_type &operator()(const PromisesInfoShrdPtr &p) const;
                    };
                };
                
                //tag
                struct tag_req_id{};
                struct tag_req_ts{};
                
                //multi-index set
                typedef boost::multi_index_container<
                PromisesInfo::PromisesInfoShrdPtr,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_id>,  PromisesInfo::extract_index>,
                boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_ts>,  PromisesInfo::extract_req_ts>
                >
                > SetPromise;
                
                typedef boost::multi_index::index<SetPromise, tag_req_id>::type               SetPromisesReqIdxIndex;
                typedef boost::multi_index::index<SetPromise, tag_req_id>::type::iterator     SetPromisesReqIdxIndexIter;
                typedef boost::multi_index::index<SetPromise, tag_req_ts>::type               SetPromisesReqTSIndex;
                typedef boost::multi_index::index<SetPromise, tag_req_ts>::type::iterator     SetPromisesReqTSIndexIter;
                
                CHAOS_DEFINE_LOCKABLE_OBJECT(SetPromise, LSetPromise);
                
                //!promises counter
                ChaosAtomic<uint32_t>   promises_counter;
                //set that contains all promise
                LSetPromise             set_p;
                SetPromisesReqIdxIndex& set_p_req_id_index;
                SetPromisesReqTSIndex&  set_p_req_ts_index;
                
                //!inherited from chaos::common::async_central::TimerHandler
                void timeout() {
                    LSetPromiseWriteLock wl = set_p.getWriteLockObject();
                    uint64_t current_check_ts = TimingUtil::getTimeStampInMicroseconds();
                    unsigned int max_purge_check = 10;
                    for(SetPromisesReqTSIndexIter it = set_p_req_ts_index.begin(),
                        end = set_p_req_ts_index.end();
                        it != end && max_purge_check;
                        max_purge_check--){
                        //purge outdated promise
                        if(current_check_ts > (*it)->timeout_ts+TIMEOUT_PURGE_PROMISE) {
                            set_p_req_ts_index.erase(it++);
                        } else {
                            ++it;
                        }
                    }
                }
            public:
                FutureHelper():
                promises_counter(0){}
                
                virtual ~FutureHelper() {}
                
                void init(void *init_data) throw(chaos::CException) {
                    chaos::common:;async_central::AsyncCentralManager::getInstance()->addTimer(this,
                                                                                               PURGE_TIMER_REPEAT_DELAY,
                                                                                               PURGE_TIMER_REPEAT_DELAY);
                }
                
                void deinit() throw(chaos::CException) {
                    chaos::common:;async_central::AsyncCentralManager::getInstance()->removeTimer(this);
                }
                
                void setDataForPromiseID(const int64_t promise_id, T promise_data) {
                    LSetPromiseWriteLock wl = set_p.getWriteLockObject();
                    SetPromisesReqIdxIndexIter it = set_p_req_id_index.find(req_index);
                    if(it != set_p_req_id_index.end()) {
                        //set promises and remove it
                        (*it)->promise.set_value(promise_data);
                        set_p_req_id_index.erase(it);
                    }
                }
                
                void addNewPormise(uint64_t& new_promise_id, Future& new_future) {
                    //store promises in result map
                    LSetPromiseWriteLock lmr_wl = set_p.getWriteLockObject();
                    PromisesInfo::PromisesInfoShrdPtr promise_info(new PromisesInfo());
                    promise_info->timeout_ts = TimingUtil::getTimeStampInMicroseconds();
                    promise_info->promise_id = promise_id = message_counter++;
                    
                    set_p().insert(promise_info);

                    //set the future
                    new_future = promise_info->promise.get_future();
                }
            }
            
        }
    }
}

#endif /* chaos_common_utility_PromisesHelper_h */
