//
//  PluginDefinition.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/3/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/plugin/PluginInspector.h>

using namespace chaos::common::plugin;

PluginInspector::PluginInspector() {
    
}

PluginInspector::~PluginInspector() {
    initAttributeForName.clear();
}

void PluginInspector::setName(const char *_name) {
    name = _name;
}

void PluginInspector::setType(const char *_type) {
    type = _type;
}

void PluginInspector::setVersion(const char *_version) {
    version = _version;
}

void PluginInspector::setSubclass(const char *_subclass) {
    subclass = _subclass;
}

const char * const PluginInspector::getName() {
    return name.c_str();
}

const char * const PluginInspector::getType() {
    return type.c_str();
}

const char * const PluginInspector::getVersion() {
    return version.c_str();
}

const char * const PluginInspector::getSubclass() {
    return subclass.c_str();
}

void PluginInspector::addInitAttributeForName(const char *name, const char * initAttribute) {
    initAttributeForName.insert(make_pair<string, string>(name, initAttribute));
}

size_t PluginInspector::getInputAttributeByNamesSize(const char *name) {
    return initAttributeForName.count(name);
}

const char * const PluginInspector::getInputAttributeForNameAndIndex(const char *name, size_t idx) {
    std::vector<std::string> initAttributes;
    //check if name exists
    if(!initAttributeForName.count(name)) return NULL;
    
    pair<multimap<string, string>::iterator, multimap<string, string>::iterator> attributesIterator;
    attributesIterator = initAttributeForName.equal_range(name);
    
    //copy all init attribute for name
    int _idx = 0;
    for (multimap<string, string>::iterator it2 = attributesIterator.first;
         it2 != attributesIterator.second;
         ++it2) {
        if(idx==_idx++) return it2->second.c_str();
    }
    return NULL;
}