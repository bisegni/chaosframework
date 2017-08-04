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
    return 0;
}
