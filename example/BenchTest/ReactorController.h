//
//  ReactorController.h
//  BenchTest
//
//  Created by Claudio Bisegni on 1/12/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __BenchTest__ReactorController__
#define __BenchTest__ReactorController__

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <chaos/common/thread/ChaosThread.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>

using namespace chaos;
using namespace std;


typedef enum ControllerState {
    INIT,
    DEINIT
} ControllerState;

class ReactorController : public chaos::CThreadExecutionTask {
    string reactorID;
    boost::shared_ptr<CThread> chaosThread;
    chaos::ui::DeviceController *chaosReactorController;
    ControllerState state;
protected:
    virtual void executeOnThread(const string&) throw(CException);
    
public:
    ReactorController(string& _rName);
    ~ReactorController();
    void init();
    void deinit();
    void setScheduleDelay(int64_t scheduleDaly);
    void joinThread();
};

#endif /* defined(__BenchTest__ReactorController__) */
