//
//  bus.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "CommunicationChannel.h"
#include "device_proxy.h"

namespace chaos {
    CommunicationChannel::CommunicationChannel(const char*name,uint32_t uid):basichw(name,uid){
    }
    
    int CommunicationChannel::attach(ProtocolDriver*hw){
        if(hw){
            chaos::addHWToList(m_driver_list, hw);
            device_proxy* devs=hw->getDevice();
            addHWToList<device_proxy>(m_device_list, devs);
            addHWToList<ProtocolDriver>(m_driver_list, hw);
            return DevError::NOERROR;
            }
            
        return DevError::PROTOCOL_INVALID;
        
    }
    
    int CommunicationChannel::detach(ProtocolDriver*hw){
        if(hw){
            device_proxy* devs=hw->getDevice();
            if( chaos::removeHWToList(m_device_list, devs)!=0){
                DERR("error removing device\n");
                return DevError::PROTOCOL_DETACH_ERROR;
            }
            if( chaos::removeHWToList(m_driver_list, hw)!=0){
                DERR("error removing protocol\n");
                return DevError::PROTOCOL_DETACH_ERROR;
            }
        }
        
        return DevError::PROTOCOL_INVALID;
    }
    
    std::vector< ProtocolDriver* >& CommunicationChannel::getDriverList(){
        return m_driver_list;
    }
    std::vector< device_proxy* >& CommunicationChannel::getDeviceList(){
        return m_device_list;
    }


}
