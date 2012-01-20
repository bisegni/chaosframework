//
//  HLInfrastructureApi.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_HLInfrastructureApi_h
#define ChaosFramework_HLInfrastructureApi_h

#include <chaos/ui_toolkit/Common/DeviceAddress.h>

namespace chaos{
    namespace ui{
        class HLInfrastructureApi {
            friend class ChaosUIToolkit;
            
            /*
             * Constructor
             */
            HLInfrastructureApi();
            
            /*
             * Distructor
             */
            ~HLInfrastructureApi();
            
        public:
            /*
             return the number and the pointer to the array of devices 
             that are contained on the specifed host and published to
             specified port
             */
            void getDevicesOnIpAndPort(DeviceAddress*, int*);
        };        
    }
}

#endif
