//
//  main.cpp
//  TestAsyncRpc
//
//  Created by Claudio Bisegni on 19/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <string>
#include <iostream>

#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>

using namespace chaos::common::message;

int main(int argc, char* argv[] ) {

    //! [UIToolkit Init]
    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
    //! [UIToolkit Init]
    chaos::ui::DeviceController *controller = chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(std::string("rt_sin_a"), 10000);
    if(!controller) throw chaos::CException(4, "Error allcoating decive controller", "device controller creation");

    //simulate bad init call
    CDataWrapper *tmp_data_ptr = NULL;
    if(controller->checkRPCInformation(&tmp_data_ptr) == 0){
        if(tmp_data_ptr) {
            std::cout << tmp_data_ptr->getJSONString() << std::endl;
            delete(tmp_data_ptr);
        }
    }
    
    chaos::ui::ChaosUIToolkit::getInstance()->deinit();
    return 0;
}

