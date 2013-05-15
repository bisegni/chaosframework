//
//  main.cpp
//  testSWAlim
//
//  Created by andrea michelotti on 4/30/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include <iostream>
//#include <curses.h>
#include "SWAlim.h"
#include "ModbusProtocol.h"
#include "TcpChannel.h"
#include <stdio.h>
using namespace chaos;

void run(SWAlim& myalim){
    int current=0,voltage=0;
    int c=0;
    while(1){
       
        myalim.readCurrent(current);
        myalim.readVoltage(voltage);
        printf("(1) on (2) off (3) start (4) lvl (5) set current, current: %8d, voltage %8d\r",current,voltage);
        fflush(stdout);
        //c = std::cin.get();
        //c=getch();
        if(c == '1') myalim.on();
        if(c == '2') myalim.off();
        if(c == '3') myalim.start();
        if(c == '4') myalim.moveLevel();
        if(c == '5') {
            int current;
            printf("\nset current to:");
            scanf("%d",&current);
            myalim.setCurrent(current);
            printf("\n");
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    TcpChannel tcpChan("LocalTcp");
    ModbusProtocol prot("my modbus");
    SWAlim myalim("SWAlim00","localhost:8083");
    
    prot.attach(&tcpChan);
    myalim.attach(&prot);
    if(myalim.init()==0){
        run(myalim);
    }
    
    return 0;
}

