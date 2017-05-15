/*
 *	chaos_types.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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

#define  CHAOS_DEFINE_SET_FOR_TYPE(t1, n)\
typedef std::set< t1 >                   n;\
typedef std::set< t1 >::iterator         n ## Iterator;\
typedef std::set< t1 >::const_iterator   n ## ConstIterator;

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

#define chaos_unique_ptr std::auto_ptr
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
