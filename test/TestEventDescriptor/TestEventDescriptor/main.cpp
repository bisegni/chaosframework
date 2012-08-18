//
//  main.cpp
//  TestEventDescriptor
//
//  Created by Claudio Bisegni on 20/08/12.
//  Copyright (c) 2012 Claudio Bisegni. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/EventFactory.h>

#define HEX( x ) setw(2) << setfill('0') << hex << (int)( x )

void printInfoMemory(chaos::event::EventDescriptor *test) {
    uint8_t ver = 0;
    uint16_t len = 0;
    std::cout.unsetf(ios::hex);
    std::cout << "vers = " << (int)(ver=test->getEventHeaderVersion()) << "\n";
    std::cout << "data len = " << (int)(len=test->getEventDataLength()) << "\n";

    const unsigned char * testMemory = test->getEventData();
    for (int i=1; i < len+1; i++) {
        std::cout << HEX(testMemory[i-1]);
        if(i%20 == 0 || i==(len)) std::cout << std::endl;
        if(i%4 == 0 ) std::cout << " ";
    }
}

int main(int argc, const char * argv[])
{

    chaos::event::alert::AlertEventDescriptor * test = new chaos::event::alert::AlertEventDescriptor();
    chaos::event::EventDescriptor * test2 = NULL;
    test->initData();
    
    int8_t value = 125;
    test->setAlert(chaos::event::alert::EventAlertThresholdCrossing, UINT16_MAX, chaos::event::EventDataInt8, &value);
    test->getAlertValue(&value, sizeof(int8_t));
    std::cout << "First Test" << std::endl;
    printInfoMemory(test);
    
    test2 = chaos::event::EventFactory::getEventInstance(test->getEventData(), test->getEventDataLength());
    std::cout << "Second Test" << std::endl;
    printInfoMemory(test2);
    
    if(test) delete(test);
    if(test2) delete(test2);
    
    return 0;
}

