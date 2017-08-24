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

#ifndef CHAOSFramework_chaos_types_h
#define CHAOSFramework_chaos_types_h

#include <chaos/common/batch_command/BatchCommandTypes.h>

#include <boost/ptr_container/ptr_container.hpp>

#include <boost/thread.hpp>

#include <set>
#include <map>
#include <queue>
#include <deque>
#include <vector>


#if __cplusplus >= 201103L
#ifndef FORCE_BOOST_SHPOINTER
#define ChaosSharedPtr      std::shared_ptr
#define ChaosMakeSharedPtr  std::make_shared
#define ChaosWeakPtr        std::weak_ptr
#include <atomic>
#include <future>
#include <chrono>
template<typename T>
using ChaosAtomic = std::atomic<T>;
template<typename T>
using ChaosPromise = std::promise<T>;
template<typename T>
using ChaosFuture = std::future<T>;
typedef std::future_status ChaosFutureStatus;
typedef std::chrono::seconds ChaosCronoSeconds;
typedef std::chrono::milliseconds ChaosCronoMilliseconds;
typedef std::chrono::microseconds ChaosCronoMicroseconds;
#else
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/future.hpp>
#include <boost/chrono.hpp>
#define ChaosSharedPtr boost::shared_ptr
#define ChaosMakeSharedPtr boost::make_shared
#define ChaosWeakPtr boost::weak_ptr
#define ChaosAtomic boost::atomic;
#define ChaosPromise boost::promise
#define ChaosFuture boost::shared_future
#define ChaosFutureStatus boost::future_status
#define ChaosCronoSeconds boost::chrono::seconds
#define ChaosCronoMilliseconds boost::chrono::milliseconds
#define ChaosCronoMicroseconds boost::chrono::microseconds
#endif
#define ChaosUniquePtr std::unique_ptr
#define ChaosMoveOperator(x) std::move(x)
#else
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/future.hpp>
#include <boost/chrono.hpp>
#define ChaosSharedPtr boost::shared_ptr
#define ChaosMakeSharedPtr boost::make_shared
#define ChaosWeakPtr boost::weak_ptr
#define ChaosUniquePtr std::auto_ptr
#define ChaosMoveOperator(x) x
#define ChaosAtomic boost::atomic
#define ChaosPromise boost::promise
#define ChaosFuture  boost::shared_future
#define ChaosFutureStatus boost::future_status
#define ChaosCronoSeconds boost::chrono::seconds
#define ChaosCronoMilliseconds boost::chrono::milliseconds
#define ChaosCronoMicroseconds boost::chrono::microseconds
#endif

#define  CHAOS_DEFINE_SET_FOR_TYPE(t1, n)\
typedef std::set< t1 >                   n;\
typedef std::set< t1 >::iterator         n ## Iterator;\
typedef std::set< t1 >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_SET_FOR_TYPE_COMP(t1, c, n)\
typedef std::set< t1 , c>                   n;\
typedef std::set< t1 , c>::iterator         n ## Iterator;\
typedef std::set< t1 , c>::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_VECTOR_FOR_TYPE(t, n)\
typedef std::vector< t >                   n;\
typedef std::vector< t >::iterator         n ## Iterator;\
typedef std::vector< t >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_VECTOR_FOR_TYPE_IN_TEMPLATE(t, n)\
typedef std::vector< t >                    n;\
typedef typename std::vector< t >::iterator         n ## Iterator;\
typedef typename std::vector< t >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_QUEUE_FOR_TYPE(t, n)\
typedef std::queue< t >                   n;\
typedef std::queue< t >::iterator         n ## Iterator;\
typedef std::queue< t >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_DEQUE_FOR_TYPE(t, n)\
typedef std::deque< t >                   n;\
typedef std::deque< t >::iterator         n ## Iterator;\
typedef std::deque< t >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_PTR_VECTOR_FOR_TYPE(t, n)\
typedef boost::ptr_vector< t >                   n;\
typedef boost::ptr_vector< t >::iterator         n ## Iterator;\
typedef boost::ptr_vector< t >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_MAP_FOR_TYPE(t1, t2, n)\
typedef std::map< t1, t2 >                   n;\
typedef std::map< t1, t2 >::iterator         n ## Iterator;\
typedef std::map< t1, t2 >::const_iterator   n ## ConstIterator;\
typedef std::pair< t1, t2 >                  n ## Pair;

#define  CHAOS_DEFINE_MAP_FOR_TYPE_IN_TEMPLATE(t1, t2, n)\
typedef std::map< t1, t2 >                      n;\
typedef typename std::map< t1, t2 >::iterator           n ## Iterator;\
typedef typename std::map< t1, t2 >::const_iterator     n ## ConstIterator;\
typedef typename std::pair< t1, t2 >                    n ## Pair;

#define  CHAOS_DEFINE_MMAP_FOR_TYPE(t1, t2, n)\
typedef std::multimap< t1, t2 >                   n;\
typedef std::multimap< t1, t2 >::iterator         n ## Iterator;\
typedef std::multimap< t1, t2 >::const_iterator   n ## ConstIterator;

#define  CHAOS_DEFINE_PTR_MAP_FOR_TYPE(t1, t2, n)\
typedef boost::ptr_map< t1, t2 >                   n;\
typedef boost::ptr_map< t1, t2 >::iterator         n ## Iterator;\
typedef boost::ptr_map< t1, t2 >::const_iterator   n ## ConstIterator;

#define CHAOS_DEFINE_GET_PTR_MAP_ELEMENT(map, element_type, element_key)\
dynamic_cast<element_type*>(&map.at(element_key));

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, ChaosStringVector)
CHAOS_DEFINE_SET_FOR_TYPE(std::string, ChaosStringSet)

#define CHAOS_SCAN_VECTOR_ITERATOR(iter, vec, to_execute)\
for(iter it = vec.begin();\
it != vec.end();\
it++) {\
to_execute \
}\

typedef boost::shared_mutex                     ChaosSharedMutex;
typedef boost::shared_lock<boost::shared_mutex> ChaosReadLock;
typedef boost::unique_lock<boost::shared_mutex> ChaosWriteLock;

typedef struct {
    float push_rate; // push rate
    float sys_time; // time spent in system
    float usr_time; // time spent in user
    uint64_t upt_time; // uptime seconds
    uint64_t metric_time; // timestamp of the metric
} cu_prof_t;

#endif
