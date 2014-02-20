//
//  CacheDriver.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 15/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "CacheDriver.h"

using namespace chaos::data_service::cache_system;

CacheDriver::CacheDriver(std::string alias) : NamedService(alias) {}

CacheDriver::~CacheDriver() {}