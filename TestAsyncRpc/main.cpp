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

void handler(chaos::common::utility::atomic_int_type message_id, chaos::common::data::CDataWrapper *result) {
    LAPP_ << message_id;
    if(result) {
        LAPP_ << result->getJSONString();
        delete result;
    }
    
}

int main(int argc, char* argv[] ) {

    //! [UIToolkit Init]
    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
    //! [UIToolkit Init]
    chaos::ui::DeviceController *controller = chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(std::string("sc_dev_a"), 10000);
    if(!controller) throw chaos::CException(4, "Error allcoating decive controller", "device controller creation");

    //simulate bad init call
    std::auto_ptr<MessageRequestFuture> result = controller->sendCustomRequestWithFuture(chaos::NodeDomainAndActionRPC::ACTION_NODE_INIT, NULL);
    if(result->wait(1000)) {
        std::cout << "Error code:" << result->getError() << std::endl;
        std::cout << "Error message:" << result->getErrorMessage() << std::endl;
        std::cout << "Error domain:" << result->getErrorDomain() << std::endl;
        if(result->getResult()) {
            std::cout << "Result:" << result->getResult()->getJSONString() << std::endl;
        }

    }
    chaos::ui::ChaosUIToolkit::getInstance()->deinit();
    return 0;
}

