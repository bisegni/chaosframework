    //
    //  main.cpp
    //  ControlSystemLibTest
    //
    //  Created by Claudio Bisegni on 13/02/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include <iostream>
#include <string>

#include "WorkerCU.h"
#include <chaos/common/cconstants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/cu_toolkit/ConfigurationManager/ConfigurationManager.h>


using namespace std;
using namespace chaos;

int main (int argc, const char* argv[] )
{
    ChaosCUToolkit csl;
    
    csl.init(argc, argv);
    
        //add at this time the Custom CU
    csl.addControlUnit(new WorkerCU());
        //the call start
    csl.start();
    return 0;
}
