//
//  main.cpp
//  ChaosMicroUnitToolkitTest
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>
#include <chaos_micro_unit_toolkit/data/DataPack.h>

using namespace chaos::micro_unit_toolkit::data;

int main(int argc, const char * argv[]) {
    DataPack dp;
    DataPack dp_parent;
    dp.addBoolValue("b", true);
    dp.addInt32Value("ti32", 32);
    dp.addInt64Value("ti64", 32);
    dp.addDoubleValue("double", 90.5);
    dp_parent.addStringValue("skey", "strings");
    dp_parent.addDataPackValue("dpkey", dp);
    dp_parent.createArrayForKey("array");
    dp_parent.appendDataPackValue("array", dp);
    dp_parent.appendDataPackValue("array", dp);
    dp_parent.appendDataPackValue("array", dp);
    
    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(5);
    dp_parent.addArrayValue("iarr", arr);
    std::cout << dp_parent.toString() << std::endl;
    return 0;
}
