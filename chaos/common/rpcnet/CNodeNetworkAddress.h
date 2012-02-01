//
//  NetworkAddress.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 28/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_NetworkAddress_h
#define ChaosFramework_NetworkAddress_h

#include <string>
namespace chaos {
        using namespace std;
    //! Represent the abstraction of chaos address of the node
    /*! \class DeviceAddress
     this permit to abtsrac the chaos address of single node througt rpc system
     */
        struct CNodeNetworkAddress{
                //the ipo for the host that run the control unit
            string ipPort;
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

