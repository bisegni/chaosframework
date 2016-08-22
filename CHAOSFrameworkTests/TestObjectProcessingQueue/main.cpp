//
//  main.cpp
//  TestObjectProcessingQueue
//
//  Created by bisegni on 22/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestObjectProcessingQueue.h"

int main(int argc, const char * argv[]) {
    chaos::common::pqueue::test::TestObjectProcessingQueue oqt;
    if(!(oqt.test(10, 100, 100, 0, true) == true)) return 1;
}
