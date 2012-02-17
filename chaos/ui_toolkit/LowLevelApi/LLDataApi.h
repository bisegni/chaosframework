/*	
 *	LLDataApi.h
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
#ifndef ChaosFramework_LLDataApi_h
#define ChaosFramework_LLDataApi_h

#include <string>
#include <chaos/common/utility/Singleton.h>
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
        class LLDataApi : public Singleton<LLDataApi>  {
            friend class ChaosUIToolkit;
            static DeviceDatasetCache *datasetCache;
            IODataDriver *liveDriver;
            
            LLDataApi();
            ~LLDataApi();
            
          public:          
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
