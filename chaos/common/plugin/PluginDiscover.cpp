//
//  PluginDiscover.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/3/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/plugin/PluginDiscover.h>

using namespace chaos::common::plugin;

PluginDiscover::PluginDiscover() {
    
}

PluginDiscover::~PluginDiscover() {
    names.clear();
}

void PluginDiscover::addName(const char *name) {
    names.push_back(name);
}

size_t PluginDiscover::getNamesSize() {
    return names.size();
}

const char * const PluginDiscover::getNameForIndex(size_t idx) {
    const std::string& result = names[idx];
    return result.c_str();
}