/*	
 *	HLDataApi.h
 *	!CHAOS
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
#ifndef ChaosFramework_HLDataApi_h
#define ChaosFramework_HLDataApi_h
#include <map>
#include <string>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <chaos/common/message/PerformanceNodeChannel.h>
#include <chaos/common/utility/Singleton.h>
namespace chaos_message = chaos::common::message;
namespace chaos {
    namespace ui {
        using namespace std;
        /*
         High level api for maning device datasert
         */
		class HLDataApi:
		public common::utility::Singleton<HLDataApi> {
            friend class ChaosUIToolkit;
            friend class common::utility::Singleton<HLDataApi>;
            
            std::map<string, DeviceController* > controllerMap;
            /*
             LL Rpc Api static initialization it should be called once for application
             */
            void init() throw (CException);
            /*
             Deinitialization of LL rpc api
             */
            void deinit() throw (CException);
            
            /*
             * Constructor
             */
            HLDataApi();
            
            /*
             * Distructor
             */
            ~HLDataApi();
            
        public:
            
            DeviceController *getControllerForDeviceID(string deviceID, uint32_t controller_timeout=1000) throw (CException);
            void disposeDeviceControllerPtr(DeviceController *) throw (CException);
			
			void createNewSnapshot(const std::string& snapshot_name);
			
			void deleteSnapshot(const std::string& snapshot_name);
        };
    }
}
#endif
