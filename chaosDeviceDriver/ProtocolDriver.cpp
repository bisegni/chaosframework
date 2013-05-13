//
//  ProtocolDriver.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/18/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "ProtocolDriver.h"
#include "CommunicationChannel.h"
#include "basichw.h"
namespace chaos{
    ProtocolDriver::ProtocolDriver(const char*name,uint32_t uid,CommunicationChannel*comm):m_bus(comm),basichw(name,uid){
    }
    
    ProtocolDriver::ProtocolDriver(const char*name):m_bus(0),basichw(name,ProtocolDriverUid::NO_PROTOCOL){
    }
    
    int ProtocolDriver::attach(CommunicationChannel *hw){
        if(hw){
            m_bus = hw;
            if(m_dev == NULL){
                DERR("Protocol must be attached to a device");
                return DevError::BUS_ATTACH_ERROR;
            }
            return hw->attach(this);
        }
        return DevError::BUS_ATTACH_ERROR;
    }
    
    int ProtocolDriver::detach(CommunicationChannel *hw){
        if(hw && m_bus == hw){
            m_bus = NULL;
            return hw->detach(this);
        }
        return DevError::BUS_DETACH_ERROR;
    }

    int ProtocolDriver::attach(device_proxy *hw){
        if(hw){
            m_dev = hw;
            if(m_bus)
                return m_bus->attach(this);
            return DevError::NOERROR;
        }
        return DevError::DEVICE_ATTACH_ERROR;
    }
    
    int ProtocolDriver::detach(device_proxy *hw){
        if(hw){
            int ret ;
            if(m_bus){
                ret= m_bus->detach(this);
            }
            m_dev = NULL;
        }
        return DevError::DEVICE_ATTACH_ERROR;
    }

    device_proxy* ProtocolDriver::getDevice(){
        return m_dev;
    }
    
    CommunicationChannel*ProtocolDriver::getCommunicationChannel(){
        return m_bus;
    }
    
    

};
