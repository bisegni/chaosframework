//
//  UIToolkitCMDLineExample.cpp
//  UIToolkitCMDLineExample
//
//  Created by Claudio Bisegni on 24/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <iostream>
#include <string>

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
    
    CDataWrapper *request=new CDataWrapper();
    request->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, "SIMULATED_DEVICE_ID");
    auto_ptr<CDataWrapper>response(mdsChannel->sendRequest("system", "getCurrentDataset", request));
    
    //! [UITOOLKIT_EX1 Init]
    ChaosUIToolkit::getInstance()->deinit();
    //! [UITOOLKIT_EX1 Init]
    return 0;
}
