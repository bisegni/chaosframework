//
//  main.cpp
//  TestEventDescriptor
//
//  Created by Claudio Bisegni on 20/08/12.
//  Copyright (c) 2012 Claudio Bisegni. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/common/event/channel/AlertEventChannel.h>

using namespace chaos;
using namespace chaos::ui;

#define HEX( x ) setw(2) << setfill('0') << hex << (int)( x )

void printInfoMemory(chaos::event::EventDescriptor *test) {
    uint8_t ver = 0;
    uint16_t len = 0;
    std::cout.unsetf(ios::hex);
    std::cout << "vers = " << (int)(ver=test->getEventHeaderVersion()) << "\n";
    std::cout << "data len = " << (int)(len=test->getEventDataLength()) << "\n";
    std::cout << "evt rpiority = " << (int)(test->getEventPriority()) << "\n";
    std::cout << "subcode = " << (int)(test->getSubCode()) << "\n";
    std::cout << "subcode priority = " << (int)(test->getSubCodePriority()) << "\n";
    std::cout << "event value type = " << (event::EventDataType)(test->getEventValueType()) << "\n";
    std::cout << "event value size = " << (int)(test->getEventValueSize()) << "\n";
    const unsigned char * testMemory = test->getEventData();
    for (int i=1; i < len+1; i++) {
        std::cout << HEX(testMemory[i-1]);
        if(i%20 == 0 || i==(len)) std::cout << std::endl;
        if(i%4 == 0 ) std::cout << " ";
    }
}

int main(int argc, char * argv[])
{

    try {
        chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
        chaos::event::channel::AlertEventChannel *alertChannel = LLRpcApi::getInstance()->getNewAlertEventChannel();
        
        /*
        auto_ptr<event::alert::AlertEventDescriptor> dsc(new event::alert::AlertEventDescriptor());
        auto_ptr<event::alert::AlertEventDescriptor> dsc2(new event::alert::AlertEventDescriptor());
        dsc->initData();
        uint16_t testSize = 0;
        uint32_t testValue = 32;
        dsc->setAlert("device_a", strlen("device_a"), event::alert::EventAlertThresholdCrossing, 200, event::EventDataInt32, &testValue);
        printInfoMemory(dsc.get());
        
        testSize = testValue = 0;
        dsc->getEventValue(&testValue, &testSize);
        
        dsc2->setEventData(dsc->getEventData(), dsc->getEventDataLength());
        printInfoMemory(dsc2.get());
*/
        alertChannel->activateChannelEventReception();
        alertChannel->sendEventInt32("device_a", event::alert::EventAlertThresholdCrossing, 200, 256);

        sleep(5);
        LLRpcApi::getInstance()->disposeEventChannel(alertChannel);
    } catch (chaos::CException& ex) {
        
    }
    ChaosUIToolkit::getInstance()->deinit();
    return 0;
}

