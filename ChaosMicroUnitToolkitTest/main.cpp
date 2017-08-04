//
//  main.cpp
//  ChaosMicroUnitToolkitTest
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;

int main(int argc, const char * argv[]) {
    DataPack dp;
    DataPack dp_parent;
    dp.addBool("b", true);
    dp.addInt32("ti32", 32);
    dp.addInt64("ti64", 32);
    dp.addDouble("double", 90.5);
    dp_parent.addString("skey", "strings");
    dp_parent.addDataPack("dpkey", dp);
    dp_parent.createArrayForKey("array");
    dp_parent.appendDataPack("array", dp);
    dp_parent.appendDataPack("array", dp);
    dp_parent.appendDataPack("array", dp);
    
    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(5);
    dp_parent.addArray("iarr", arr);
    std::cout << dp_parent.toUnformattedString() << std::endl;
    
    ChaosMicroUnitToolkit mut;
    const char *option="Content-Type: application/bson-json\r\n";
    ChaosUniquePtr< UnitConnection<RawDriverUnitProxy> > proxy = mut.createNewRawDriverUnit(ProtocolTypeHTTP,
                                                                                            "ws://localhost:8080/io_driver",
                                                                                            option);
    
    int err = proxy->protocol_adapter->connect();
    proxy->protocol_adapter->poll(1000);
    if(err) return err;

    proxy->unit_proxy->authorization("work");
    proxy->protocol_adapter->poll(2000);
    if(proxy->unit_proxy->hasMoreMessage()) {
        RemoteMessageUniquePtr m = proxy->unit_proxy->getNextMessage();
        std::cout << m->message->toString() << std::endl;
    }
    proxy->protocol_adapter->close();
    
    return 0;
}
