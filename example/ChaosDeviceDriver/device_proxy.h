//
//  device_proxy
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/16/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__Device__
#define __chaosDeviceDriver__Device__

#include <iostream>
#include <map>
#include <stdint.h>
#include "DriverUID.h"
#include "basichw.h"
#include "ProtocolDriver.h"
#include "CommunicationChannel.h"
#include <stdio.h>

namespace chaos {

    namespace DevError {
    enum {
        NOERROR,
        
        PROTOCOL_ERROR,
        PROTOCOL_ATTACH_ERROR,
        PROTOCOL_DETACH_ERROR,
        PROTOCOL_INVALID,
        PROTOCOL_NOFOUND,
        
        BUS_ERROR,
        BUS_ATTACH_ERROR,
        BUS_DETACH_ERROR,
        BUS_INIT_ERROR,
        BUS_WRITE_ERROR,
        BUS_INVALID,
        
        DEVICE_ERROR,
	DEVICE_BAD_ADDRESS,
        DEVICE_ATTACH_ERROR,
        DEVICE_DETACH_ERROR,
        DEVICE_INVALID
        };
    };
    class device_proxy : public basichw {
    
    protected:
        // identifier of the device inside a bus
        std::string m_busid;
        // attached driver
        ProtocolDriver* m_driver;
        
    public:
        device_proxy(){m_driver=NULL;}
        device_proxy(const char*n,const char*_busid,uint32_t uid=DeviceUid::NO_DEVICE):basichw(n,uid),m_busid(_busid){
            
        }

    device_proxy(const char*n):basichw(n,DeviceUid::NO_DEVICE){
            
        }

        const char* getBusID();
        void setBusID(const char*busid);
        template<typename T>
        T* getProtocol(){return static_cast<T*>(m_driver);}
    
        /**
         attach a Protocol driver
         
         @param hw Protocol driver to attach
         @return 0 on success
         */
        int attach(ProtocolDriver* hw);
        
        /**
         detach a Protocol driver from this device
         
         @param hw to detach
         @return 0 on success
         */
        int detach(ProtocolDriver* hw);
        
        int init();
        int deinit();

        size_t read(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo=0,uint32_t flags=0){return 0;}
        
        size_t write(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo=0,uint32_t flags=0){return 0;}

    };
};

#endif /* defined(__chaosDeviceDriver__Device__) */
