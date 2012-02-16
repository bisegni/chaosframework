    //
    //  CThreadGroup.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef CThreadGroup_H
#define CThreadGroup_H

#include <boost/thread.hpp>
#include "CThread.h"

namespace chaos{
    using namespace boost;
    using namespace std;
    
    class CThreadGroup {
        bool stopped;
        friend class CThread;
        thread_group threadGroup;
        vector<CThread *> threadVec;
        long threadRunning;
        boost::mutex notify_thr_end_mutex;
        
        void threadHasFinished(CThread*);
    public:
        CThreadGroup();
        ~CThreadGroup();
        
        void addThread(CThread *controlUnitThread);
        void removeThread(CThread *controlUnitThread);
        CThread *getThread(int);
        void joinGroup();
        void setDelayBeetwenTask(long);
        void startGroup();
        void stopGroup(bool join=true);
        bool groupHasFinisched();
        bool isStopped();
    };
}
#endif
