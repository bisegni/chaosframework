//
//  AbstractApi.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 14/01/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "AbstractApi.h"

using namespace chaos::wan_proxy::api;

AbstractApi::AbstractApi(const std::string& name):
NamedService(name){}

AbstractApi::~AbstractApi() {}