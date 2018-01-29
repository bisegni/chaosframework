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

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define TIMEOUT_PURGE_PROMISE 30000
#define PURGE_TIMER_REPEAT_DELAY 30000

namespace chaos{
    namespace common {
        namespace thread {
            
            template<typename T>
            class FutureHelper:
            public chaos::common::utility::InizializableService,
            public chaos::common::async_central::TimerHandler {
            public:
                typedef uint32_t CounterType;
                //!define the types
                typedef ChaosPromise< T >  Promise;
                typedef ChaosSharedPtr< Promise > PromiseUPtr;
                typedef ChaosSharedFuture< T >      Future;
            private:
                struct PromisesInfo;
                //typedef ChaosSharedPtr< PromisesInfo > PromisesInfoShrdPtr;
                
                struct PromisesInfo {
                    PromiseUPtr promise;
                    int64_t timeout_ts;
                    CounterType promise_id;
                    
                    PromisesInfo():
                    promise(new Promise()){}
                    
                    PromisesInfo(const PromisesInfo& src):
                    timeout_ts(src.timeout_ts),
                    promise_id(src.promise_id),
                    promise(src.promise){}
                    ~PromisesInfo() {}
                    
                    bool operator<(const PromisesInfo& p)const{
                        DBG_LOG(PromisesInfo::operator) << p.promise_id;
                        return promise_id<p.promise_id;
                    }
                    //!key accessors for multindix infrastructure
                    struct extract_index {
                        typedef CounterType result_type;
                        const result_type &operator()(const PromisesInfo &p) const {
                            DBG_LOG(PromisesInfo::extract_index) << p.promise_id;
                            return p.promise_id;
                        }
                    };
                    
                    struct extract_req_ts {
                        typedef int64_t result_type;
                        const result_type &operator()(const PromisesInfo &p) const {
                            DBG_LOG(PromisesInfo::extract_req_ts) << p.timeout_ts;
                            return p.timeout_ts;
                        }
                    };
                };
                
                struct PromisesInfoPromiseIdComparator {
                    bool operator()(const CounterType& lhs, const CounterType& rhs) const {
                        if(lhs == rhs ) return false;
                        return lhs < rhs;
                    }
                };
                
                struct PromisesInfoPromiseTSComparator {
                    bool operator()(const int64_t& lhs, const int64_t& rhs) const {
                        if(lhs == rhs ) return false;
                        return lhs < rhs;
                    }
                };
                
                //tag
                struct tag_req_id{};
                struct tag_req_ts{};
                
                //multi-index set
                typedef boost::multi_index_container<
                PromisesInfo,
                boost::multi_index::indexed_by<
                //boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_id>,  typename PromisesInfo::extract_index>,
                //boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_ts>,  typename PromisesInfo::extract_req_ts>
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<tag_req_id>, BOOST_MULTI_INDEX_MEMBER(PromisesInfo, CounterType, promise_id), PromisesInfoPromiseIdComparator >,
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<tag_req_ts>, BOOST_MULTI_INDEX_MEMBER(PromisesInfo, int64_t, timeout_ts), PromisesInfoPromiseTSComparator >
                >
                > SetPromise;
                
                typedef typename boost::multi_index::index<SetPromise, tag_req_id>::type               SetPromisesReqIdxIndex;
                typedef typename boost::multi_index::index<SetPromise, tag_req_id>::type::iterator     SetPromisesReqIdxIndexIter;
                typedef typename boost::multi_index::index<SetPromise, tag_req_ts>::type               SetPromisesReqTSIndex;
                typedef typename boost::multi_index::index<SetPromise, tag_req_ts>::type::iterator     SetPromisesReqTSIndexIter;
                
                typedef typename chaos::common::utility::LockableObject<SetPromise> LSetPromise;
                typedef typename chaos::common::utility::LockableObject<SetPromise>::LockableObjectReadLock LSetPromiseReadLock;
                typedef typename chaos::common::utility::LockableObject<SetPromise>::LockableObjectWriteLock LSetPromiseWriteLock;
                
                const uint32_t promise_timeout;
                const uint32_t purge_delay;
                //!promises counter
                ChaosAtomic<CounterType>   promises_counter;
                //set that contains all promise
                LSetPromise             set_p;
                SetPromisesReqIdxIndex& set_p_req_id_index;
                SetPromisesReqTSIndex&  set_p_req_ts_index;
                
                //!inherited from chaos::common::async_central::TimerHandler
                void timeout() {
                    LSetPromiseWriteLock wl = set_p.getWriteLockObject();
                    uint64_t current_check_ts = chaos::common::utility::TimingUtil::getTimeStamp();
                    DEBUG_CODE(LDBG_ << DEFINE_LOG_HEADER(FutureHelper) << __FUNCTION__ << " - Start cleaning promises";);
                    unsigned int max_purge_check = 10;
                    for(SetPromisesReqTSIndexIter it = set_p_req_ts_index.begin(),
                        end = set_p_req_ts_index.end();
                        it != end && max_purge_check;
                        max_purge_check--){
                        //purge outdated promise
                        if(current_check_ts >= (*it).timeout_ts) {
                            LDBG_ << DEFINE_LOG_HEADER(FutureHelper) << __FUNCTION__ << CHAOS_FORMAT(" - Remove the promise for request of index %1%", %(*it).promise_id);
                            set_p_req_ts_index.erase(it++);
                        } else {
                            ++it;
                        }
                    }
                }
            public:
                FutureHelper(const uint32_t& _purge_delay = PURGE_TIMER_REPEAT_DELAY,
                             const uint32_t& _promise_timeout = TIMEOUT_PURGE_PROMISE):
                promise_timeout(_promise_timeout),
                purge_delay(_purge_delay),
                promises_counter(0),
                set_p_req_id_index(boost::multi_index::get<tag_req_id>(set_p())),
                set_p_req_ts_index(boost::multi_index::get<tag_req_ts>(set_p())){}
                
                virtual ~FutureHelper() {}
                
                void init(void *init_data) throw(chaos::CException) {
                    chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this,
                                                                                               purge_delay,
                                                                                               purge_delay);
                }
                
                void deinit() throw(chaos::CException) {
                    chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);
                    LSetPromiseWriteLock lmr_wl = set_p.getWriteLockObject();
                    set_p().clear();
                }
                
                void setDataForPromiseID(const CounterType promise_id, T promise_data) {
                    LSetPromiseReadLock wl = set_p.getReadLockObject();
                    SetPromisesReqIdxIndexIter it = set_p_req_id_index.find(promise_id);
                    if(it != set_p_req_id_index.end()) {
                        //set promises and remove it
                        (*it).promise->set_value(promise_data);
                        set_p_req_id_index.erase(it);
                    }
                }
                
                void addNewPromise(CounterType& new_promise_id, Future& new_future) {
                    //store promises in result map
                    LSetPromiseWriteLock lmr_wl = set_p.getWriteLockObject();
                    PromisesInfo promise_info;
                    promise_info.timeout_ts = chaos::common::utility::TimingUtil::getTimestampWithDelay(promise_timeout, true);
                    promise_info.promise_id = new_promise_id = promises_counter++;
                    new_future = promise_info.promise->get_future();
                    std::pair<SetPromisesReqIdxIndexIter,bool> ires = set_p().insert(promise_info);
                    //for debug print al the element
                    SetPromisesReqIdxIndex& id_idx = boost::multi_index::get<tag_req_id>(set_p());
                    SetPromisesReqIdxIndexIter it = id_idx.find(new_promise_id);
                    if(it == id_idx.end()) {
                        LDBG_ << DEFINE_LOG_HEADER(FutureHelper) << __FUNCTION__ << CHAOS_FORMAT("Promises with index %1% has not be added colliding with %2%", %new_promise_id%(*ires.first).promise_id);
                    }
                }
            };
        }
    }
}

#endif /* chaos_common_utility_PromisesHelper_h */
