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
        DeviceMessageChannel(MessageBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress);
        
    public:
        void setNewAddress(CDeviceNetworkAddress *_deviceAddress);
            //!Initialization of the device
        /*!
         Perform the hardware initialization
         \param initData the pointer to the data for device initialization. The memory is not deallocate after the send operation
         \millisecToWait the number of millisecond for waith the answer
         */
        int initDevice(CDataWrapper *initData, uint32_t millisecToWait = 0);
            //!Device deinititalization
        /*!
         Perform the hardware deinitialization
         \millisecToWait the number of millisecond for waith the answer
         */
        int deinitDevice(uint32_t millisecToWait = 0);
            //!Start the device
        /*!
         Perform the hardware start of scheduling
         \millisecToWait the number of millisecond for waith the answer
        */
        int startDevice(uint32_t millisecToWait = 0);
            //! Stop the device
        /*!
         Perform the hardware stop of the scheduling
         \millisecToWait the number of millisecond for waith the answer
         */
        int stopDevice(uint32_t millisecToWait = 0);
            //! Get device state
        /*!
         Get the current state of the hardware
         \millisecToWait the number of millisecond for waith the answer
         */
        int getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait = 0);
        
            //! Send the CDatawrapper as device attribute values
        /*!
         All the CDataWrapper is sent as pack for the device attributes values, no check is done
         \param attributesValues the container for the values of some dataset attributes
         */
        int setAttributeValue(CDataWrapper& attributesValues, uint32_t millisecToWait = 0);
        
            //!Set the device thread schedule delay
        /*!
         Set the delay between a schedule and the next
         */
        int setScheduleDelay(uint32_t scheduledDealy, uint32_t millisecToWait = 0);
    };
}
#endif
