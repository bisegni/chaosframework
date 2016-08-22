//
//  main.cpp
//  TestURLFeeder
//
//  Created by bisegni on 22/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestURLFeeder.h"
int main(int argc, const char * argv[]) {
    chaos::test::network::TestURLFeeder fd;
    fd.test(100000);
    return 0;
}
