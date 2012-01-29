//
//  UIToolkitCMDLineExample.cpp
//  UIToolkitCMDLineExample
//
//  Created by Claudio Bisegni on 24/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <iostream>
#include <string>

#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
/*! \page page_example_uiex1 ChaosUIToolkit Example
    \section page_example_uiex1_sec A basic usage for the ChaosUIToolkit package
 
    \subsection page_example_uiex1_sec_sub1 Toolkit usage
    this example show how initialize the UIToolkit, acquire a basic channel, used it and deinit the toolkit.
    The UIToolkit is create around singleton pattern and the channel object are self managed by toolkit. So the first thing
    is to initializ the toolkit interna engine:

    \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
    the argc, argv parameter are the common c and cpp main param.
    
    For comunicate with the chaos resurces, a channel is needed. For example to get device infromation we need to ask it
    to metadataserver. To instantiate a basic channel to cominicate with it must be used the LLRpcApi api package. To achieve channel creation
    must be used the singleton instance for calling the method "getNewMetadataServerChannel"
    \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit ChannelCreation
    The returned pointer to MessageChannel is managed by LLRpcApi
    
    Every action in chaos system require that param are encoded into a CDataWrapper class, so for get the device infromation we need to call the
    metadataserver rpc method "getCurrentDataset" lcoated in "system" domain. In the example the id associated with device is "SIMULATED_DEVICE_ID"
    \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp Datapack creation
 
    At this point a request can be sent to metadata server and we must wait for the answer. For that we can use "sendRequest" method of MessageChannel
    class and wait the answer.
    \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp Datapack sent
 */
using namespace std;
using namespace chaos;
using namespace chaos::ui;

int main (int argc, const char* argv[] )
{
    try {
            //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
            //! [UIToolkit Init]
        
            //! [UIToolkit ChannelCreation]
        MessageChannel *mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
            //! [UIToolkit ChannelCreation]
        
            //! [Datapack creation]
        CDataWrapper *request=new CDataWrapper();
        request->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, "SIMULATED_DEVICE_ID");
            //! [Datapack creation]
        
            //! [Datapack sent]
        auto_ptr<CDataWrapper>response(mdsChannel->sendRequest("system", "getCurrentDataset", request));
        std::cout << response->getJSONString() << std::endl;
            //! [Datapack sent]
        
            //realloc the memory
        request=new CDataWrapper();
        request->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, "SIMULATED_DEVICE_ID");
        response.reset(mdsChannel->sendRequest("system", "getCurrentDataset", request, 200));
        
        if(response.get())
            std::cout << response->getJSONString() << std::endl;
        
        request=new CDataWrapper();
        request->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, "SIMULATED_DEVICE_ID");
        response.reset(mdsChannel->sendRequest("system", "getCurrentDataset", request));
        
        if(response.get())
            std::cout << response->getJSONString() << std::endl;
        
        
        ChaosUIToolkit::getInstance()->deinit();

    } catch (CException& e) {
        std::cerr<< e.errorDomain << std::endl;
        std::cerr<< e.errorMessage << std::endl;
    }
    return 0;
}
