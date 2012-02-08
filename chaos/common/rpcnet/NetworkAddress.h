//
//  NetworkAddress.h
//  ControlSystemLib
//
//  Created by Bisegni Claudio on 28/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ControlSystemLib_NetworkAddress_h
#define ControlSystemLib_NetworkAddress_h

#include <string>
namespace chaos {
        using namespace std;
        /*
         Manage an address for a specified endpoint
         */
        struct DeviceAddress {
                //the ipo for the host that run the control unit
            string ipPort;
                //the instance of control unit that contain the deviceid
            string cuInstance;
                //the key that identify the device
            string deviceID;
        };
}
#endif

