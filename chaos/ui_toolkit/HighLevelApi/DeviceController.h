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
#include <chaos/common/utility/IDataBuffer.h>

namespace chaos {
    namespace ui{
        
            //! Controller for a single device instance
        /*! 
         This represent a global controller for a single device, identified by DEVICE_ID. The contorlle rpermit to
         contorl the initialization and operation phase for a device. Allow to send value for input dataset and read the last 
         freshenest value form the live data
         */
        class DeviceController: public CThreadExecutionTask {
            friend class HLDataApi;
                //! represent the device id controlled by this instance
            string deviceID;
                //! the haos address of device
            auto_ptr<CDeviceNetworkAddress> deviceAddress;
                //! Metadata Server channel for get device information
            MDSMessageChannel *mdsChannel;
                //! Device MEssage channel to control via chaos rpc the device
            DeviceMessageChannel *deviceChannel;
                //! The io driver for accessing live data of the device
            IODataDriver *ioLiveDataDriver;
                //!the thread for the fetch operation
            CThread *liveDataThread;
            
            auto_ptr<CDataWrapper> currentLiveValue;
                //! Defautl Constructor
            /*!
             The visibility of the constructor is private becase it's is isntantiable only via HLDataApi singleton
             */
            DeviceController(string& _deviceID);
            
                //! update inromation for talking with device
            /*!
             Perform oall the orpeation to find the rigth chaos address of the device
             */
            void updateChannel() throw(CException);
            
            /*!
             Fetch the current live value form live storage
             */
            inline void fetchCurrentDeviceValue();
            
            /*!
             Initialize the map for the devices
             \param initiDevicedescription the reference to CDataWrapper that contain device initialization information
             */
            void initializeAttributeIndexMap(CDataWrapper& initiDevicedescription);
        protected:
                //! the fetcher thread method
            void executeOnThread(const string&) throw(CException);
        public:
                //!Public destructor
            /*!
             All can destruct an isntance of the device controller
             */
            ~DeviceController();
            
                //!Device initialization
            /*!
             Perform the device initialization phase
             */
            int initDevice();
            
                //!Start the device chaos driver acquisition data scheduler
            /*!
             Perform the device start pahse. Thsi phase represent the run methdo called at certain delay in  athread
             */
            int startDevice();
            
                //!Stop(pause) the device driver acquisition data scheduler
            /*!
             Perform the device initialization phase
             */
            int stopDevice();
            
                //!Device deinitialization phase
            /*!
             Perform, if it's not be done the stop operation and afther the hardware deinitialization
             */
            int deinitDevice();
            
                //!Set the scheduler delay for this determinated device
            /*!
             Set the delay for the thread that call the run() methdo for this device
             \param millisecDelay the delay from the query to the metadata server
             */
            int setScheduleDelay(int32_t millisecDelay);
            
                //!Start to trackthe current device value
            /*!
             Start the live data tracking for this device, if no delay is given, the default delay of the device on metadataserver is got
             */
            void startTracking();
            
                //!Stop the live data tracking
            /*!
             Interrupt the live data tracking operation
             */
            void stopTracking();
            
                //!Add a new value for indexed attribute to buffer
            /*! 
             \param buffer buffer where to put a new value retrieved fro live
             \param indexOfAttribute index of the attribute to fetch
             */
            template<typename T>
            void addLiveValueToBuffer(typename choas::IDataBuffer<T> buffer, int32_t indexOfAttribute[]) {
                fetchCurrentDeviceValue();
                if(!currentLiveValue.get())return;
            }
        };
    }
}

#endif
