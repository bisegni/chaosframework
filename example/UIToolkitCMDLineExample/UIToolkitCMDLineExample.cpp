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
 
    \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UITOOLKIT_EX1 Init 
 */
using namespace std;
using namespace chaos;
using namespace chaos::ui;

int main (int argc, const char* argv[] )
{
    
    //! [UITOOLKIT_EX1 Init]
    ChaosUIToolkit::getInstance()->init(argc, argv);
    //! [UITOOLKIT_EX1 Init]
    
    MessageChannel *mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
    
    LAPP_<< "Prepared request 0";
    CDataWrapper *request=new CDataWrapper();
    request->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, "SIMULATED_DEVICE_ID");
    
        //this method dealloc the requestMemory
    auto_ptr<CDataWrapper>response(mdsChannel->sendRequest("system", "getCurrentDataset", request));
    LAPP_<< "request 0 returned";
    std::cout << response->getJSONString() << std::endl;
    
        //realloc the memory
    LAPP_<< "Prepared request 1";
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
    
    //! [UITOOLKIT_EX1 Init]
    ChaosUIToolkit::getInstance()->deinit();
    //! [UITOOLKIT_EX1 Init]
    return 0;
}
