/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef ChaosFramework_HLDataApi_h
#define ChaosFramework_HLDataApi_h
#include <map>
#include <string>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <chaos/common/message/PerformanceNodeChannel.h>
#include <chaos/common/utility/Singleton.h>

namespace chaos {
    namespace ui {
        /*
         High level api for maning device datasert
         */
		class HLDataApi:
		public common::utility::Singleton<HLDataApi> {
            friend class ChaosUIToolkit;
            friend class common::utility::Singleton<HLDataApi>;
            
            std::map<std::string, DeviceController* > controllerMap;
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
