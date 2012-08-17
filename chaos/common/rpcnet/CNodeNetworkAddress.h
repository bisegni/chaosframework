/*	
 *	NetworkAddress.h
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

#ifndef ChaosFramework_NetworkAddress_h
#define ChaosFramework_NetworkAddress_h

#include <string>
namespace chaos {
        using namespace std;
        //! Represent the abstraction of network address of the physical node
    /*! \class DeviceAddress
     this permit to abtsrac the chaos address of single node througt rpc system
     */
    struct CNetworkAddress {
            //the ip and port for the host that run the control unit
        string ipPort;
    };
    
    //! Represent the abstraction of chaos node id of the chaos virtual node
    /*! \class DeviceAddress
     this permit to abtsrac the chaos address of single node througt rpc system
     */
    struct CNodeNetworkAddress : public CNetworkAddress {
                //the instance of control unit that contain the deviceid
            string nodeID;
        };
 
    //! Represent the abstraction of chaos address of the node
    /*! \class DeviceAddress
     this permit to abtsrac the chaos address of single node througt rpc system
     */
        struct CDeviceNetworkAddress : public CNodeNetworkAddress{
            //the ipo for the host that run the control unit
            string deviceID;
        };
}
#endif
