//
//  main.cpp
//  TestVariant
//
//  Created by bisegni on 08/05/2017.
//  Copyright © 2017 bisegni. All rights reserved.
//

#include <cassert>
#include <iostream>
#include <algorithm>
#include <chaos/common/data/CDataVariant.h>

using namespace chaos::common::data;

bool SafeLess( int x, unsigned int y )
{
    return (x < 0) || (static_cast<unsigned int>(x) < y);
}

bool SafeLess( unsigned int x, int y )
{
    return (y >= 0) && (x < static_cast<unsigned int>(y));
}

int main(int argc, const char * argv[]) {
    CDataVariant str_v("35.5");
    CDataVariant neg_str_v("-35.5");
    CDataVariant double_v(35.5);
    CDataVariant neg_double_v(-35.5);
    
    //str variant
    assert(35.5 == static_cast<double>(str_v));
    assert(35 == static_cast<int32_t>(str_v));
    assert(true == static_cast<bool>(str_v));
    assert(strcmp("35.5", static_cast< boost::shared_ptr<CDataBuffer> >(str_v)->getBuffer()) == 0);
    
    //neg str variant
    assert(-35.5 == static_cast<double>(neg_str_v));
    assert(-35 == static_cast<int32_t>(neg_str_v));
    assert(true == static_cast<bool>(neg_str_v));
    assert(strcmp("-35.5", static_cast< boost::shared_ptr<CDataBuffer> >(neg_str_v)->getBuffer()) == 0);

    //double variant
    assert(35.5 == static_cast<double>(double_v));
    assert(35 == static_cast<int32_t>(double_v));
    assert(true == static_cast<bool>(double_v));
    
    //double neg variant
    assert(-35.5 == static_cast<double>(neg_double_v));
    assert(-35 == static_cast<int32_t>(neg_double_v));
    assert(true == static_cast<bool>(neg_double_v));
    return 0;
}
