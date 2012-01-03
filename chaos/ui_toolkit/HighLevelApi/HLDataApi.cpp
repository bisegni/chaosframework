//
//  HLDataApi.cpp
//  ControlSystemLib
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "HLDataApi.h"

using namespace std;
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


/*
 Return a channel for a device by his device_id key
 */
DeviceDataChannel *HLDataApi::getDeviceLiveDataChannelByDeviceID(string& deviceKey) {
    return NULL;
}

/*
 Return a channel for a device by his ip:port address
 */
DeviceDataChannel *HLDataApi::getDeviceLiveDataChannelByAddress(DeviceAddress& deviceAddress) {
    return NULL;
}