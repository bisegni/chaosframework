    //
    //  CThreadGroup.cpp
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

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
        //threadGroup.join_all();
    for (vector<CThread*>::iterator simThrIter=threadVec.begin() ; simThrIter < threadVec.end(); simThrIter++ ){
        (*simThrIter)->join();
    }
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
