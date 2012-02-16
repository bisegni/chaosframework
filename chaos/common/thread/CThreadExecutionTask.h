    //
    //  ThreadExecutionTask.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef ThreadExecutionTask_H
#define ThreadExecutionTask_H

#include <chaos/common/exception/CException.h>
namespace chaos {
    class CThread;
    class CThreadExecutionTask {
        friend class CThread;
    protected:
        virtual void executeOnThread(const string&) throw(CException) = 0;
    };
}
#endif

