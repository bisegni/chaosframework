//
//  DeviceLiveDataFetcher.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 07/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DeviceLiveDataFetcher_h
#define CHAOSFramework_DeviceLiveDataFetcher_h

#include <chaos/common/rpcnet/CNodeNetworkAddress.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/thread/CThread.h>
#include <chaos/common/thread/CThreadExecutionTask.h>
namespace chaos {
    namespace ui{
        
        class DeviceController: public CThreadExecutionTask {
            friend class HLDataApi;
            string deviceID;
            auto_ptr<CDeviceNetworkAddress> deviceAddress;
            MDSMessageChannel *mdsChannel;
            DeviceMessageChannel *deviceChannel;
            IODataDriver *ioLiveDataDriver;
            CThread *liveDataThread;
            
            DeviceController(string& _deviceID);
            void updateChannel() throw(CException);
        protected:
            void executeOnThread(const string&) throw(CException);
        public:
            ~DeviceController();
            int initDevice();
            int startDevice();
            int stopDevice();
            int deinitDevice();
            int setScheduleDelay(int64_t millisecDelay);
            void startTracking();
            void stopTracking();
        };
    }
}

#endif
