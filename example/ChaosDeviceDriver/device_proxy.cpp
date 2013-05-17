//
//  Device.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/16/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "device_proxy.h"

namespace chaos {
    const char* device_proxy::getBusID(){
        return m_busid.c_str();
    }
    void device_proxy::setBusID(const char*busid){
        m_busid = busid;
    }
int device_proxy::attach(ProtocolDriver* hw){
    if(hw){
        m_driver = hw;
        return hw->attach(this);
    }
    return DevError::DEVICE_ATTACH_ERROR;
}

    int device_proxy::detach(ProtocolDriver* hw){
            if(hw==m_driver){
                m_driver = NULL;
                return hw->detach(this);
            }
        return DevError::DEVICE_DETACH_ERROR;
    }
    
    int device_proxy::init(){
        if (m_driver)
            return m_driver->init();
        return DevError::PROTOCOL_INVALID;
        
    }
    int device_proxy::deinit(){
        if (m_driver)
            return m_driver->init();
        return DevError::PROTOCOL_INVALID;
        
    }
}
