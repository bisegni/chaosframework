//
//  MainOrkestrator.h
//  BenchTest
//
//  Created by Claudio Bisegni on 1/13/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __BenchTest__MainOrkestrator__
#define __BenchTest__MainOrkestrator__

#include <vector>
#include <string>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/ArrayPointer.h>
#include "ReactorController.h"

class MainOrkestrator {
    bool waithUntilEnd;
    std::vector< std::string > *reactordeviceIDs;
    std::vector< double > *reactorsReferements;
    std::vector< uint64_t > *reactorSimulatedSpeed;
    chaos::ArrayPointer< ReactorController > reactorControllerList;
    
    static chaos::WaitSemaphore waitCloseSemaphore;
    

    static void signalHanlder(int);
    
public:
    typedef boost::mutex::scoped_lock lock;
 
    MainOrkestrator(std::vector< std::string > *_reactordeviceIDs, std::vector< double > *_reactorsReferements, std::vector< uint64_t > *_simulatedSpeed);
    ~MainOrkestrator();
    void init();
    void join();
};


#endif /* defined(__BenchTest__MainOrkestrator__) */
