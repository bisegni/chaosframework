//
//  main.cpp
//  ChaosNodeDirectory
//
//  Created by Claudio Bisegni on 03/11/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <iostream>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include "ChaosNodeDirectory.h"

using namespace chaos;

int main(int argc, char * argv[]) {
        //! Setup custom param for Directory Node
    chaos::nd::ChaosNodeDirectory::getInstance()->getGlobalConfigurationInstance()->addOption("device_a", po::value<string>(), "Device A identification string");
        //Init the Node
    chaos::nd::ChaosNodeDirectory::getInstance()->init(argc, argv);
        //!Start the node
    chaos::nd::ChaosNodeDirectory::getInstance()->start();
    return 0;
}

