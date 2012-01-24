//
//  HLDataApi.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_HLDataApi_h
#define ChaosFramework_HLDataApi_h

#include <string>
#include <chaos/ui_toolkit/Common/DeviceDataChannel.h>
#include <chaos/ui_toolkit/Common/DeviceAddress.h>
namespace chaos {
    namespace ui {
        using namespace std;
        /*
         High level api for maning device datasert
         */
        class HLDataApi {
            friend class ChaosUIToolkit;
        
        /*
         * Constructor
         */
        HLDataApi();
        
        /*
         * Distructor
         */
        ~HLDataApi();

        public:
            /*
             Return a channel for a device by his device_id key
             */
            DeviceDataChannel *getDeviceLiveDataChannelByDeviceID(string&);
            
            /*
             Return a channel for a device by his ip:port address
             */
            DeviceDataChannel *getDeviceLiveDataChannelByAddress(DeviceAddress&);

        };
    }
}



#endif
