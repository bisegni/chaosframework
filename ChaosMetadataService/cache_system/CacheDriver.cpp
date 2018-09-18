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
#include "CacheDriver.h"

using namespace chaos::metadata_service::cache_system;

CacheDriver::CacheDriver(std::string alias):
NamedService(alias),
cache_settings(NULL){}

CacheDriver::~CacheDriver() {}

//! init
/*!
 Need a point to a structure DBDriverSetting for the setting
 */
void CacheDriver::init(void *init_data)  {
	cache_settings = static_cast<CacheDriverSetting*>(init_data);
}

//! deinit
void CacheDriver::deinit()  {
	
}
