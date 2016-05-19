/*
 *	CacheDriver.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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
#include "CacheDriver.h"

using namespace chaos::data_service::cache_system;

CacheDriver::CacheDriver(std::string alias):
NamedService(alias),
cache_settings(NULL){}

CacheDriver::~CacheDriver() {}

//! init
/*!
 Need a point to a structure DBDriverSetting for the setting
 */
void CacheDriver::init(void *init_data) throw (chaos::CException) {
	cache_settings = static_cast<CacheDriverSetting*>(init_data);
}

//! deinit
void CacheDriver::deinit() throw (chaos::CException) {
	
}