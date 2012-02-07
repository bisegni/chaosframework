//
//  CThread.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 20/03/11.
//  Copyright 2011 INFN. All rights reserved.
//



#include "../global.h"
#include "CThread.h"
#include "CThreadGroup.h"

using namespace chaos;
using namespace boost;
using namespace boost::chrono;

CThread::CThread(){
    internalInit();
}

CThread::CThread(CThreadExecutionTaskSPtr tUnit){
    setTask(tUnit);
    internalInit();
}

CThread::~CThread(){
    
}

void CThread::internalInit() {
    threadIdentification = "";
    firstLoop = true;
	m_stop = true;
	parentCThreadGroup = 0L;
    waithTimeInMicrosecond = microseconds(0);
}

// Create the CThread and start work
void CThread::start() {
    if(!m_stop) return;
    m_stop = false;
    statisticData.ptimeNextStart = statisticData.ptimeStart = boost::chrono::steady_clock::now();
    m_thread.reset(new thread(boost::bind(&CThread::executeWork, this)));
	statisticData.cycle = 0;
}

void CThread::stop(bool joinCThread) {
    assert(m_thread);
    if(m_stop) return;
    
    m_stop = true;
    if(joinCThread) m_thread->join();
}

bool CThread::isStopped() {
    return m_stop;
}

void CThread::setDelayBeetwenTask(long msec) {
    waithTimeInMicrosecond = microseconds(msec);
}

void CThread::join() {
    m_thread->join();
}

void CThread::setThreadIdentification(string& _threadIdentification) {
	threadIdentification = _threadIdentification;
}

//set CThread id
void CThread::setThreadPriorityLevel(int priorityLevel, int policyLevel) {
#if defined(__linux__) || defined(__APPLE__)
    int retcode;
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) m_thread->native_handle();
    if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)  {
        errno = retcode;
#if DEBUG
        LDBG_<<"pthread_getschedparam" << errno;
#endif
        return;
    }
#if DEBUG
    LDBG_ << "PreSET: ";
    LDBG_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                           (policy == SCHED_RR)    ? "SCHED_RR" :
                           (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                           "???")
    << ", priority=" << param.sched_priority << std::endl;
#endif
    
    policy = policyLevel;
    param.sched_priority = priorityLevel;
    if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0) {
        errno = retcode;
#if DEBUG
        LDBG_ << "pthread_setschedparam" << errno;
#endif
    }
    
    if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0) {
        errno = retcode;
#if DEBUG
        LDBG_<<"pthread_getschedparam" << errno;
#endif
        return;
    }
#if DEBUG
    LDBG_ << "PostSet: ";
    LDBG_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                           (policy == SCHED_RR)    ? "SCHED_RR" :
                           (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                           "???")
    << ", priority=" << param.sched_priority << std::endl;
#endif
    
#endif
}

void CThread::setTask(CThreadExecutionTaskSPtr tUnit) {
    taskUnit = tUnit;
}

TaskCycleStatPtr CThread::getStat() {
	TaskCycleStatPtr resultStat(new TaskCycleStatData());
    //get the mutex lock
	mutex::scoped_lock  lock(statMutex);
	memcpy(resultStat.get(), (void*)&statisticData, sizeof(TaskCycleStatData));
	return resultStat;
}

void CThread::executeWork() {
    microseconds toWaith;
    try{
        while (!m_stop) {
            //mutex::scoped_lock  lock(statMutex);
            for (; boost::chrono::steady_clock::now()<statisticData.ptimeNextStart; );
            statisticData.ptimeNextStart = boost::chrono::steady_clock::now() + waithTimeInMicrosecond;
            
            //call scheduled function
            taskUnit->executeOnThread(threadIdentification);
            //taskUnit->executeOnThread();
            
            //se if we need to whait for the nex execution
            if(waithTimeInMicrosecond.count()>=0 && !m_stop) {
                //count the nanosecond to waith
                if(waithTimeInMicrosecond.count() > 0){
                    toWaith = duration_cast<microseconds>(statisticData.ptimeNextStart - boost::chrono::steady_clock::now());
                    boost::this_thread::sleep(boost::posix_time::microseconds(toWaith.count() - 500));
                }
            } else {
                //if m_stop==true or waithTimeInMicrosecond is < 0
                //then we fall here so CThread will exit
                m_stop = true;
            }
        }
    }catch(CException& exc){
         m_stop = true;
        //some exception has occured during thread execution
        DECODE_CHAOS_EXCEPTION(exc);
    }
	if(parentCThreadGroup) parentCThreadGroup->threadHasFinished(this);
}   