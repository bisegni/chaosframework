//
//  NamedService.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "NamedService.h"

using namespace chaos;
using namespace std;

NamedService::NamedService(string *alias) {
    typeName = alias;
}

/*
 Return the adapter alias
 */
const char * NamedService::getName() const {
    return typeName->c_str();
}