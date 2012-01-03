//
//  DeviceAddress.h
//  ControlSystemLib
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosLib_DeviceAddress_h
#define ChaosLib_DeviceAddress_h
#include <string>
namespace chaos {
    namespace ui {
        using namespace std;
        /*
         Manage an address for a specified deviceId
         */
        struct DeviceAddress {
                //the ipo for the host that run the control unit
            string ip;
                //the port where is publish the control unit instance
            int  port;
                //the instance of control unit that contain the deviceid
            string cuInstance;
                //the key that identify the device
            string deviceID;
            
        };
    }
}
#endif
