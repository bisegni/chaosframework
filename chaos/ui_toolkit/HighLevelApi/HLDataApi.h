//
//  HLDataApi.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_HLDataApi_h
#define ChaosFramework_HLDataApi_h

#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <chaos/common/utility/Singleton.h>
namespace chaos {
    namespace ui {
        using namespace std;
        /*
         High level api for maning device datasert
         */
        class HLDataApi  : public Singleton<HLDataApi> {
            friend class ChaosUIToolkit;
            friend class Singleton<HLDataApi>;
        /*
         * Constructor
         */
        HLDataApi();
        
        /*
         * Distructor
         */
        ~HLDataApi();

        public:
            DeviceController *getControllerForDeviceID(string& deviceID) throw (CException);
        };
    }
}
#endif
