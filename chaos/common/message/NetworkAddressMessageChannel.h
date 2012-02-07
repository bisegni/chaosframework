//
//  NetworkAddressMessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_NetworkAddressMessageChannel_h
#define CHAOSFramework_NetworkAddressMessageChannel_h
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

#include <vector>

namespace chaos {
    
    using namespace std;
    
    class MessageBroker;
    
    //! Message Channel constructed with a node address
    /*! 
     This represent a subclass of MessageChannel that is addess with an instance of
     CNodeNetworkAddress ans information for the endpoint
     */
    class NetworkAddressMessageChannel : protected MessageChannel {
        friend class MessageBroker;
    protected:
        //! node address for metadata server
        auto_ptr<CNodeNetworkAddress> nodeAddress;

    public:
        //!Base constructor
        /*!
         Perform the node notwork construction
         \param msgBroker the broker used by this channel
         \param mdsIpAddress the address of metdataserver
         */
        NetworkAddressMessageChannel(MessageBroker *msgBroker,  CNodeNetworkAddress *_nodeAddress) : MessageChannel(msgBroker) {
            CHAOS_ASSERT(_nodeAddress)
            setNewAddress(_nodeAddress);           
        }
        
        virtual ~NetworkAddressMessageChannel(){
                //if(nodeAddress) delete nodeAddress;
        }
        
        virtual void setNewAddress(CNodeNetworkAddress *_nodeAddress) {
            nodeAddress.reset(_nodeAddress);
            if(_nodeAddress)setRemoteNodeAddress(nodeAddress->ipPort);
        }
    };

}
#endif
