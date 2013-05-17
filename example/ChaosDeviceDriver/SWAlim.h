//
//  SWAlim.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/30/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__SWAlim__
#define __chaosDeviceDriver__SWAlim__

#include <iostream>
#include "device_proxy.h"


namespace chaos{
    class SWAlim:public device_proxy {
        enum {
            SET_ON = 1,
            SET_OFF = 2,
            START = 4,
            MOVE_LVL =8,
            SET_CURRENT =9,

            
        };
        
    private:
        int command(int cmdid, int val=0);
        
    public:
         
	SWAlim(const char*name);
        SWAlim(const char*name,const char* busid);
        int init();
        int deinit();
        
        int readCurrent(int& curr);
        int readVoltage(int&volt);
        int on();
        int off();
        int start();
        int moveLevel();
        int setCurrent(int curr);
    };
};
#endif /* defined(__chaosDeviceDriver__SWAlim__) */
