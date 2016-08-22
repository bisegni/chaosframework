//
//  main.cpp
//  TestPriorityQueue
//
//  Created by bisegni on 22/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//

#include "TestObjectProcessingPriorityQueue.h"
#include "TestObjectProcessingPriorityQueueForPriority.h"

int main(int argc, const char * argv[]) {
    
    chaos::common::pqueue::test::TestObjectProcessingPriorityQueue opqt;
    chaos::common::pqueue::test::TestObjectProcessingPriorityQueueForPriority opqt_priority;
    if(!opqt.test(10, 100, 100, 0, true)) return 1;
    if(!opqt_priority.test(50, 1000)) return 1;
    return 0;
}
