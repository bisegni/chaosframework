//
//  HLDataApi.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "HLDataApi.h"

using namespace std;
using namespace chaos;
using namespace chaos::ui;

/*
 * Constructor
 */
HLDataApi::HLDataApi() {
    
}

/*
 * Distructor
 */
HLDataApi::~HLDataApi() {
    
}

DeviceController *HLDataApi::getControllerForDeviceID(string& deviceID) throw (CException) {
    DeviceController *deviceController = new DeviceController(deviceID);
    deviceController->updateChannel();
    return deviceController;
}
