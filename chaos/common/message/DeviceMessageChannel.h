//
//  DeviceMessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DeviceMessageChannel_h
#define CHAOSFramework_DeviceMessageChannel_h

#include <string>
#include <chaos/common/message/NetworkAddressMessageChannel.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

namespace chaos {
    
    using namespace std;
    
    //! Message Channel specialize for metadataserver comunication
    /*! 
     This class represent a message chanel for comunication with a device
     */
    class DeviceMessageChannel : public NetworkAddressMessageChannel {
        friend class MessageBroker;
        
        CDeviceNetworkAddress *deviceNetworkAddress;
        
    protected:
        //! base constructor
        /*!
         The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
         ip:port:cu_node_address(instance):deviceID
         */
        DeviceMessageChannel(MessageBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress) : NetworkAddressMessageChannel(msgBroker, _deviceNetworkAddress){
            //take the reference for avoid continue cast fro CNetwork and CDevice
            deviceNetworkAddress = _deviceNetworkAddress;
        }
        
    public:
        /*!
         Perform the hardware initialization
         \param initData the pointer to the data for device initialization. The memory is not deallocate after the send operation
         \millisecToWait the number of millisecond for waith the answer
         */
        int initDevice(CDataWrapper *initData, uint32_t millisecToWait = 0);
        
        /*!
         Perform the hardware deinitialization
         \millisecToWait the number of millisecond for waith the answer
         */
        int deinitDevice(uint32_t millisecToWait = 0);
        
        /*!
         Perform the hardware start of scheduling
         \millisecToWait the number of millisecond for waith the answer
        */
        int startDevice(uint32_t millisecToWait = 0);
        
        /*!
         Perform the hardware stop of the scheduling
         \millisecToWait the number of millisecond for waith the answer
         */
        int stopDevice(uint32_t millisecToWait = 0);
    };
}
#endif
