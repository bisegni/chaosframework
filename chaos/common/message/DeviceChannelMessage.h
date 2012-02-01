//
//  DeviceChannelMessage.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DeviceChannelMessage_h
#define CHAOSFramework_DeviceChannelMessage_h

#include <string>
#include <chaos/common/message/NetworkAddressMessageChannel.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

namespace chaos {
    
    using namespace std;
    
    //! Message Channel specialize for metadataserver comunication
    /*! 
     This class represent a message chanel for comunication with a device
     */
    class DeviceChannelMessage : public NetworkAddressMessageChannel {
        friend class MessageBroker;
        
        CDeviceNetworkAddress *deviceNetworkAddress;
        
    protected:
        //! base constructor
        /*!
         The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
         ip:port:cu_node_address(instance):deviceID
         */
        DeviceChannelMessage(MessageBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress) : NetworkAddressMessageChannel(msgBroker, deviceNetworkAddress){
            //take the reference for avoid continue cast fro CNetwork and CDevice
            deviceNetworkAddress = _deviceNetworkAddress;
        }
        
    public:
        
    };
}
#endif
