//
//  ProtocolDriver.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/18/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__ProtocolDriver__
#define __chaosDeviceDriver__ProtocolDriver__

#include <iostream>
#include "basichw.h"
#include <vector>
namespace chaos{
    class CommunicationChannel;
    class device_proxy;
    
       
    class ProtocolDriver:public basichw {
        
    protected:
        // one protocol driver for each bus
        CommunicationChannel* m_bus;
        
        // device attached to this protocol driver
        device_proxy* m_dev;
        
    public:
        device_proxy* getDevice();
        CommunicationChannel*getCommunicationChannel();
        
        ProtocolDriver(const char*name,uint32_t uid,CommunicationChannel*comm);
        ProtocolDriver(const char*name);
        
        /**
         attach a Communication Channel
         
         @param hw Protocol driver to attach
         @return 0 on success
         */
        int attach(CommunicationChannel* hw);
        
        /**
         detach a Communication Channel driver from this device
         
         @param hw to detach
         @return 0 on success
         */
        int detach(CommunicationChannel* hw);

        
        /**
         attach a device
         
         @param hw device to attach
         @return 0 on success
         */
        int attach(device_proxy* hw);
        
        /**
         detach a device from this device
         
         @param hw to detach
         @return 0 on success
         */
        int detach(device_proxy* hw);

        
               
    };
};
#endif /* defined(__chaosDeviceDriver__ProtocolDriver__) */
