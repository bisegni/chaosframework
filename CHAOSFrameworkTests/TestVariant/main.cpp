/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

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
    assert(strcmp("35.5", static_cast< ChaosSharedPtr<CDataBuffer> >(str_v)->getBuffer()) == 0);
    
    //neg str variant
    assert(-35.5 == static_cast<double>(neg_str_v));
    assert(-35 == static_cast<int32_t>(neg_str_v));
    assert(true == static_cast<bool>(neg_str_v));
    assert(strcmp("-35.5", static_cast< ChaosSharedPtr<CDataBuffer> >(neg_str_v)->getBuffer()) == 0);

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
