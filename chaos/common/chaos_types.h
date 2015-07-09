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

#include <map>
#include <vector>

#define  CHAOS_DEFINE_VECTOR_FOR_TYPE(t, name)\
typedef std::vector< t >                   name;\
typedef std::vector< t >::iterator         name ## Iterator;\
typedef std::vector< t >::const_iterator   name ## ConstIterator;

#define  CHAOS_DEFINE_PTR_VECTOR_FOR_TYPE(t, name)\
typedef boost::ptr_vector< t >                   name;\
typedef boost::ptr_vector< t >::iterator         name ## Iterator;\
typedef boost::ptr_vector< t >::const_iterator   name ## ConstIterator;

#define  CHAOS_DEFINE_MAP_FOR_TYPE(t1, t2, name)\
typedef std::map< t1, t2 >                   name;\
typedef std::map< t1, t2 >::iterator         name ## Iterator;\
typedef std::map< t1, t2 >::const_iterator   name ## ConstIterator;

#define  CHAOS_DEFINE_PTR_MAP_FOR_TYPE(t1, t2, name)\
typedef boost::ptr_map< t1, t2 >                   name;\
typedef boost::ptr_map< t1, t2 >::iterator         name ## Iterator;\
typedef boost::ptr_map< t1, t2 >::const_iterator   name ## ConstIterator;

#define CHAOS_DEFINE_GET_PTR_MAP_ELEMENT(map, element_type, element_key)\
dynamic_cast<element_type*>(&map.at(element_key));

#endif
