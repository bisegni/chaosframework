    //
    //  UIToolkitCMDLineExample.cpp
    //  UIToolkitCMDLineExample
    //
    //  Created by Claudio Bisegni on 24/01/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

#include <iostream>
#include <string>
#include <vector>
#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
/*! \page page_example_uiex1 ChaosUIToolkit Example
 \section page_example_uiex1_sec A basic usage for the ChaosUIToolkit package
 
 \subsection page_example_uiex1_sec_sub1 Toolkit usage
 this example show how initialize the UIToolkit, acquire a basic channel, used it and deinit the toolkit.
 The UIToolkit is create around singleton pattern and the channel object are self managed by toolkit. So the first thing
 is to initializ the toolkit interna engine:
 
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
 the argc, argv parameter are the common c and cpp main param.
 
 For comunicate with the chaos resurces, as example the metadataserver, a channel is needed. To instantiate a mds channel
 must be used the LLRpcApi api package. To achieve channel creation can be used the singleton instance for calling the method "getNewMetadataServerChannel"
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit ChannelCreation
 The returned pointer to MDSMessageChannel is interally managed, there is no need to deallocate it by and. When a channel is no more neede, the
 deallocation api must be called.
 
 At this point a request can be sent to metadata server and we must wait for the answer. For get all active devices we can use the method "getAllDeviceID"
 that get as param a vector of string, that wil be filled with all device id found
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp Datapack sent
 
 When all operation are finisched the UIToolkit ca be deinitialized, this operation will clean all pendi operation and hannel deallocation
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Deinit
 */
using namespace std;
using namespace chaos;
using namespace chaos::ui;

int main (int argc, char* argv[] )
{
    try {
        int err = 0;
            //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
            //! [UIToolkit Init]
        
            //! [UIToolkit ChannelCreation]
        MDSMessageChannel *mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
            //! [UIToolkit ChannelCreation]
        
        
            //! [Datapack sent]
        vector<string> allActiveDeviceID;
        CDeviceNetworkAddress deviceNetworkAddress;
        
        err = mdsChannel->getAllDeviceID(allActiveDeviceID, 2000);
            //! [Datapack sent]
        if(!err){
            for (vector<string>::iterator devIter = allActiveDeviceID.begin(); 
                 devIter != allActiveDeviceID.end(); 
                 devIter++) {
                std::cout << "Device Identification: " << *devIter << std::endl;
                
                auto_ptr<DeviceController> controller(HLDataApi::getInstance()->getControllerForDeviceID(*devIter));

                
                vector<string> allOutAttrName;
                 controller->getDeviceDatasetAttributesName(allOutAttrName, chaos::DataType::Output);
                
                controller->initDevice();
                controller->setScheduleDelay(1000000);
                controller->startDevice();
                controller->setupTracking();
                string key = "intValue_1";
                controller->addAttributeToTrack(key);
                DataBuffer *intValue1Buff = controller->getBufferForAttribute(key);
                controller->startTracking();
                int *bPtr = reinterpret_pointer_cast<int>(intValue1Buff->getBasePointer());
                
                for (int idx = 0; idx < 60; idx++) {
                    controller->fetchCurrentDeviceValue();
                    int *wPtr = reinterpret_pointer_cast<int>(intValue1Buff->getWritePointer());

                    std::cout << intValue1Buff->getWriteBufferPosition()<< std::endl;
                    
                    int64_t hisotryToRead = intValue1Buff->getDimension()-intValue1Buff->getWriteBufferPosition();
                    int64_t recentToRead = intValue1Buff->getWriteBufferPosition();
                    std::cout << "HIstory to read:" << hisotryToRead << std::endl;
                    std::cout << "Recent to read:" << recentToRead << std::endl;
                    

                    for (int idx = 0; idx < hisotryToRead-1; idx++) {
                        int *newbPtr=wPtr + idx;
                        std::cout << *newbPtr;
                    }
                    if(bPtr != wPtr){
                        for (int idx = 0; idx < recentToRead; idx++) {
                            int *newbPtr=bPtr + idx;
                            std::cout << *newbPtr;
                        }
                    }
                    std::cout << std::endl;
                    usleep(1000000);
                }
                
                controller->stopTracking();
                
                controller->stopDevice();
                controller->deinitDevice();
            }
        }  
            //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
            //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cerr<< e.errorDomain << std::endl;
        std::cerr<< e.errorMessage << std::endl;
    }
    return 0;
}
