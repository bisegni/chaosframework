/*	
 *	CThreadGroup.cpp
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

#include <chaos/common/global.h>
#include "CThreadGroup.h"
#include "CThread.h"
using namespace chaos;
using namespace boost;
using namespace std;

CThreadGroup::CThreadGroup() {
    threadRunning = 0;
    stopped = true;
}

CThreadGroup::~CThreadGroup() {
        //try to stop
    stopGroup();
        //clear all simulation thread
    threadVec.clear();
}

void CThreadGroup::addThread(CThread *controlUnitThread) {
    
    if((*controlUnitThread).parentCThreadGroup){
            //errore
        return;
    }
    
        //assign this thread group to thread
    (*controlUnitThread).parentCThreadGroup = this;
    
        //add simulation thread to internal vector
    threadVec.push_back(controlUnitThread);
    
        //ad the ControlUnitThread internal thread to the thread group
    threadGroup.add_thread((*controlUnitThread).m_thread.get());
}

void CThreadGroup::threadHasFinished(CThread*) {
        //get the lock for decreasing the number of runnign thread
    mutex::scoped_lock  lock(notify_thr_end_mutex);
    threadRunning--;
}

void CThreadGroup::removeThread(CThread *controlUnitThread) {
    
}

CThread *CThreadGroup::getThread(int idx) {
    return threadVec[idx];
}

void CThreadGroup::joinGroup() {
    threadGroup.join_all();
}

void CThreadGroup::setDelayBeetwenTask(long delayTime) {
    for (vector<CThread*>::iterator simThrIter=threadVec.begin() ; simThrIter < threadVec.end(); simThrIter++ ){
        (*simThrIter)->setDelayBeetwenTask(delayTime);
    }
}

/*!
 @method     startGroup
 @abstract   start all thread of the group
 @discussion
 */
void CThreadGroup::startGroup() {
    for (vector<CThread*>::iterator simThrIter=threadVec.begin() ; simThrIter < threadVec.end(); simThrIter++ ){
        (*simThrIter)->start();
            //get the lock for increasing the number of runnign thread
        mutex::scoped_lock  lock(notify_thr_end_mutex);
        threadRunning++;
    }
    stopped = false;
}

void CThreadGroup::stopGroup(bool join) {
    for (vector<CThread*>::iterator simThrIter=threadVec.begin() ; simThrIter < threadVec.end(); simThrIter++ ){
        (*simThrIter)->m_stop=true;
    }
    if(join)joinGroup();
    stopped = true;
}

bool CThreadGroup::groupHasFinisched() {
        //get the lock for read the number of runnign thread
    mutex::scoped_lock  lock(notify_thr_end_mutex);
    return threadRunning==0;
}

bool CThreadGroup::isStopped() {
    return stopped;
}
