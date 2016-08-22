//
//  main.cpp
//  TestTemplatedKeyValueHashMap
//
//  Created by bisegni on 22/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestTemplateKeyValueHashMap.h"

int main(int argc, const char * argv[]) {
    chaos::common::utility::test::TestTemplateKeyValueHashMap test;
    if(!test.test(10, 100, 10)) return 1;
    return 0;
}
