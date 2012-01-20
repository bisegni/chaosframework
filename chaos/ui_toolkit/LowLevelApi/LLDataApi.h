//
//  LLDataApi.h
//  UIToolkit
//
//  Created by Claudio Bisegni on 01/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_LLDataApi_h
#define ChaosFramework_LLDataApi_h

#include <string>

#include <chaos/ui_toolkit/Common/DeviceDatasetCache.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/io/IODataDriver.h>
namespace chaos {
    namespace ui {

        using namespace std;
        using namespace boost::interprocess;
        
        /*
         Low level api for data access api
         */
        class LLDataApi {
            friend class ChaosUIToolkit;
            static DeviceDatasetCache *datasetCache;
            IODataDriver *liveDriver;
        public:
            LLDataApi();
            ~LLDataApi();
            
            
            /*
             return the last valid dataset for a device key
             */
            char *getLastDatasetForDeviceKey(string&);
            
            int32_t getLastInt32ForDeviceKey(string&, string&);
            
            int64_t getLastInt64ForDeviceKey(string&, string&);
            
            double_t getLastDoubleForDeviceKey(string&, string&);
        };
    }
}


#endif
