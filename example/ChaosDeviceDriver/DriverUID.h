//
//  DriverUID.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/24/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef chaosDeviceDriver_DriverUID_h
#define chaosDeviceDriver_DriverUID_h
#ifdef DEBUG
#include <stdio.h>
#define DPRINT(x,...) printf( "[%lu]%s: " x,time(NULL),__func__,## __VA_ARGS__)
#else
#define DPRINT(x,...)
#endif

#define DERR(x,s...) printf("## Error in %s: " x,__func__,##s)


namespace chaos {
    
    namespace CommChannelUid {
        enum {
            NO_CHANNEL,
            TCP_CHANNEL,
            RS232_CHANNEL,
            RTU_CHANNEL,
            I2C_CHANNEL
        };
    };
    
    namespace ProtocolDriverUid {
        enum  {
            NO_PROTOCOL,
            MODBUS_TCP_PROTOCOL,
            MODBUS_RTU_PROTOCOL
        };
    };
    
    namespace DeviceUid {
        enum  {
            NO_DEVICE,
            DEVICE_SWALIM_TCP,
            DEVICE_SWALIM_RTU
        };
    };
    

};

#endif
