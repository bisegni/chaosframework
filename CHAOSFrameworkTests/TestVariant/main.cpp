//
//  main.cpp
//  TestVariant
//
//  Created by bisegni on 08/05/2017.
//  Copyright © 2017 bisegni. All rights reserved.
//

#include <iostream>
#include <chaos/common/data/CDataVariant.h>

using namespace chaos::common::data;

int main(int argc, const char * argv[]) {
    CDataVariant str_variant(-35.5);
    double  d_str_variant = str_variant;
    assert(-35.5 == d_str_variant);
    
    int32_t i32_str_variant = str_variant;
    assert(-35 == i32_str_variant);
    
    uint32_t ui32_str_variant = str_variant;
    assert(0 == ui32_str_variant);
    
    bool b_str_variant = str_variant;
    assert(true == b_str_variant);
    
    boost::shared_ptr<CDataBuffer> cdb = str_variant;
    assert(strcmp(cdb->getBuffer(), "-35.5") == 0);
    return 0;
}
