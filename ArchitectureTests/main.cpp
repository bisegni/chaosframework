//
//  main.cpp
//  EchitectureTests
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "utility/HashMapTest.h"
#include "network/FeederTest.h"
#include "thread/ObjectQueueTest.h"
#include "thread/ObjectPriorityQueueTest.h"
#include <cassert>


int main(int argc, const char * argv[])
{
    chaos::test::network::FeederTest fd;
	fd.test(100000);
    
    chaos::common::pqueue::test::ObjectQueueTest oqt;
    assert((oqt.test(10, 100, 100, 0, true) == true));
	
	chaos::common::pqueue::test::ObjectPriorityQueueTest opqt;
	assert((oqt.test(10, 100, 100, 0, true) == true));

	chaos::common::utility::test::HashMapTest hmt;
	assert(hmt.test(10, 10, 10));
    return 0;
}

