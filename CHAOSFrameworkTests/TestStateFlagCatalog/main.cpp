//
//  main.cpp
//  TestStateFlagCatalog
//
//  Created by bisegni on 22/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestStateFlagCatalog.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    chaos::common::utility::test::TestStateFlagCatalog test;
    if(!test.test()) return 1;
    return 0;
}
