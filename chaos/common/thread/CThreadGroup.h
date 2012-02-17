/*	
 *	CThreadGroup.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

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
