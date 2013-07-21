/*	
 *	MDSMessageChannel.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef CHAOSFramework_MDSMessageChannel_h
#define CHAOSFramework_MDSMessageChannel_h

#include <chaos/common/message/NodeMessageChannel.h>

#include <vector>

namespace chaos {
    
    using namespace std;
    
    //! Message Channel specialize for metadataserver comunication
    /*! 
     This class represent a message chanel for comunication with the Metadata Server 
     */
    class MDSMessageChannel : public NodeMessageChannel {
        friend class NetworkBroker;
    protected:
        //! base constructor
        /*!
         The base constructor prepare the base class constructor call to be adapted for metadataserver comunication. For the MDS the node address is
         "system"(ip:port:system)
         */
        MDSMessageChannel(NetworkBroker *msgBroker, CNodeNetworkAddress *mdsNodeAddress):NodeMessageChannel(msgBroker, mdsNodeAddress){}
        
    public:
        
        //! Send heartbeat
        /*! 
         Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance.
         The method return has fast as possible, no aswer is wait
         \param identificationID identification id of a device or a client
         */
        void sendHeartBeatForDeviceID(string& identificationID);
        
            //! Send dataset to MDS
        /*! 
         Return a list of all device id that are active
         \param deviceDataset the CDatawrapper representi the device dataset infromation, th epointer is not disposed
         \param millisecToWait delay after wich the wait is interrupt
         */
        int sendUnitDescription(CDataWrapper *deviceDataset, bool requestCheck=false, uint32_t millisecToWait=0);
        
        //! Get all active device id
        /*! 
         Return a list of all device id that are active
         \param deviceIDVec an array to contain the returned device id
         \param millisecToWait delay after wich the wait is interrupt
         */
        int getAllDeviceID(vector<string>& deviceIDVec, uint32_t millisecToWait=0);
        
        //! Get node address for identification id
        /*! 
         Return the node address for an identification id
         \param identificationID id for wich we need to get the network address information
         \param deviceNetworkAddress the hadnle to the pointer representing the node address structure to be filled with identification id network info 
         \param millisecToWait delay after wich the wait is interrupt
         \return error code
         */
        int getNetworkAddressForDevice(string& identificationID, CDeviceNetworkAddress** deviceNetworkAddress, uint32_t millisecToWait=0);
        
        //! Get curent dataset for device
        /*! 
         Return the node address for an identification id
         \param identificationID id for wich we need to get the network address information
         \param deviceDefinition this is the handle to the pointer representig the dataset desprition is returned
         \param millisecToWait delay after wich the wait is interrupt
         \return error code
         */
        int getLastDatasetForDevice(string& identificationID,  CDataWrapper** deviceDefinition, uint32_t millisecToWait=0);
    };
    
}
#endif
