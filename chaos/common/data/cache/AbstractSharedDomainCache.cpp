/*
 *	AbstractDomainCache.cpp
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

#include <chaos/common/data/cache/AbstractSharedDomainCache.h>

using namespace chaos::common::data::cache;

SharedCacheLockDomain::SharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
mutex(_mutex){
}

SharedCacheLockDomain::~SharedCacheLockDomain(){
}



WriteSharedCacheLockDomain::WriteSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
SharedCacheLockDomain(_mutex),
w_lock(*mutex.get(), boost::defer_lock){
}

WriteSharedCacheLockDomain::~WriteSharedCacheLockDomain() {
}

void WriteSharedCacheLockDomain::lock() {
	w_lock.lock();
}

void WriteSharedCacheLockDomain::unlock() {
	w_lock.unlock();
}


ReadSharedCacheLockDomain::ReadSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
SharedCacheLockDomain(_mutex),
r_lock(*mutex.get(), boost::defer_lock){
}

ReadSharedCacheLockDomain::~ReadSharedCacheLockDomain() {
}

void ReadSharedCacheLockDomain::lock() {
	r_lock.lock();
}

void ReadSharedCacheLockDomain::unlock() {
	r_lock.unlock();
}

AbstractSharedDomainCache::AbstractSharedDomainCache() {
	
}

AbstractSharedDomainCache::~AbstractSharedDomainCache() {
	
}