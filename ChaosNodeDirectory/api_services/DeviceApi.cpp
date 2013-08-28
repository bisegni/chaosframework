//
//  DeviceApi.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "DeviceApi.h"

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cnd;
using namespace chaos::cnd::api;

DeviceApi::DeviceApi() {
    DeclareAction::addActionDescritionInstance<DeviceApi>(this,
                                                          &DeviceApi::registerDevice,
                                                          "device",
                                                          "registerDevice",
                                                          "Start the registartion phae of a device");
}

DeviceApi::~DeviceApi() {
    
}

/*!
 Register a device
 */
CDataWrapper* DeviceApi::registerDevice(CDataWrapper *registrationdData, bool& detach) throw(CException) {
    if(registrationdData == NULL) throw CException(0, "The package cna't be null", "DeviceApi::registerDevice");
    
    //try to register new device if noone has been
    dm->getDeviceDatabase()->addAttributeToDataSetFromDataWrapper(*registrationdData);
    return NULL;
}