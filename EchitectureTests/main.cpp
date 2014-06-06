//
//  main.cpp
//  EchitectureTests
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//


#include "network/FeederTest.h"
#include <assert.h>


int main(int argc, const char * argv[])
{
    chaos::test::network::FeederTest fd;
	fd.test(1000);
    return 0;
}

