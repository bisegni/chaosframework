//
//  bus.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__CommunicationChannel__
#define __chaosDeviceDriver__CommunicationChannel__

#include <iostream>
#include "device_proxy.h"
#include "ProtocolDriver.h"
#include <vector>
#include <stdint.h>
namespace chaos {
    
       
    class CommunicationChannel:public basichw {
        
    protected:
    // list of drivers attached to this bus
        std::vector< ProtocolDriver* > m_driver_list;
        std::vector< device_proxy* > m_device_list;

        
    public:
        std::vector< ProtocolDriver* >& getDriverList();
        std::vector< device_proxy* >& getDeviceList();
        CommunicationChannel(){}
        CommunicationChannel(const char*name,uint32_t uid=ProtocolDriverUid::NO_PROTOCOL);
        int attach(ProtocolDriver*hw);
        int detach(ProtocolDriver*hw);  
    
    };
};

#endif /* defined(__chaosDeviceDriver__bus__) */
