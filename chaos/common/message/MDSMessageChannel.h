//
//  MDSMessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_MDSMessageChannel_h
#define CHAOSFramework_MDSMessageChannel_h

#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

#include <vector>

namespace chaos {
    
    using namespace std;
    
    //! Message Channel specialize for metadataserver comunication
    /*! 
     This is the base class for the other toolkit, it thake care to initialize all common
     resource used for the base chaos function
     */
    class MDSMessageChannel : protected MessageChannel {
        friend class MessageBroker;
        
        //! node address for metadata server
        CNodeNetworkAddress mdsAddress;
        
        //!Base constructor
        /*!
         Perform the node notwork construction
         \param msgBroker the broker used by this channel
         \param mdsIpAddress the address of metdataserver
         */
        MDSMessageChannel(MessageBroker *msgBroker, const char * const mdsIpAddress);
        
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
         */
        void getAllDeviceID(vector<string>& deviceIDVec, unsigned int millisecToWait=0);
    };
    
}

#endif
