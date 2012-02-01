//
//  MDSMessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_MDSMessageChannel_h
#define CHAOSFramework_MDSMessageChannel_h

#include <chaos/common/message/NetworkAddressMessageChannel.h>

#include <vector>

namespace chaos {
    
    using namespace std;
    
    //! Message Channel specialize for metadataserver comunication
    /*! 
     This class represent a message chanel for comunication with the Metadata Server 
     */
    class MDSMessageChannel : public NetworkAddressMessageChannel {
        friend class MessageBroker;
    protected:
        //! base constructor
        /*!
         The base constructor prepare the base class constructor call to be adapted for metadataserver comunication. For the MDS the node address is
         "system"(ip:port:system)
         */
        MDSMessageChannel(MessageBroker *msgBroker, CNodeNetworkAddress *mdsNodeAddress):NetworkAddressMessageChannel(msgBroker, mdsNodeAddress){}
        
    public:
        
        //! Send heartbeat
        /*! 
         Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance.
         The method return has fast as possible, no aswer is wait
         \param identificationID identification id of a device or a client
         */
        void sendHeartBeatForDeviceID(string& identificationID);
        
        
        //! Get all active device id
        /*! 
         Return a list of all device id that are active
         \param deviceIDVec an array to contain the returned device id
         \param millisecToWait delay after wich the wait is interrupt
         */
        int getAllDeviceID(vector<string>& deviceIDVec, unsigned int millisecToWait=0);
        
        //! Get node address for identification id
        /*! 
         Return the node address for an identification id
         \param identificationID id for wich we need to get the network address information
         \param nodeNetworkAddress node address structure to be filled with identification id network info
         \param millisecToWait delay after wich the wait is interrupt
         */
        int getNetworkAddressForDevice(string& identificationID, CDeviceNetworkAddress& deviceNetworkAddress, unsigned int millisecToWait=0);
    };
    
}

#endif
