    //
    //  CThread.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef ControlSystemThread_H
#define ControlSystemThread_H

#include <unistd.h>
#include <sched.h>
#include <boost/progress.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <chaos/common/global.h>

#include <chaos/common/thread/CThreadExecutionTask.h>

namespace chaos{
    class CThreadGroup;
    
    using namespace boost;
    using namespace boost::chrono;
    using namespace std;
    
    
    /*
     Statisticfor single contorl cycle
     */
    typedef struct {
        high_resolution_clock::time_point  ptimeStart;
        high_resolution_clock::time_point  ptimeNextStart;
        boost::chrono::nanoseconds scheduleExecution;
        boost::chrono::nanoseconds ptimeLastJitter;
            //last cycle usec duration
        long cycle;
    } TaskCycleStatData;
    
        //type for execution shared ptr
    typedef shared_ptr<TaskCycleStatData> TaskCycleStatPtr;
    typedef CThreadExecutionTask *CThreadExecutionTaskSPtr;
    /*
     Base class for control unit thread
     */
    class CThread
    {
    public:
    CThread();
    CThread(CThreadExecutionTaskSPtr);
    ~CThread();
    
        //internal init for class
    void internalInit();
    
        // Create the thread and start work
    void start();
    
        //stop the thread
    void stop(bool joinThread = true);
    
    bool isStopped();
    
        //sec to waith beetwen task execution
    void setDelayBeetwenTask(int64_t microsecond);
    
        //join to thread execution
    void join();
    
        //set thread id
	void setThreadIdentification(string&);
    
        //set thread id
	void setThreadPriorityLevel(int priorityLevel, int policyLevel=SCHED_OTHER);
    
        //set the control unit for this thread
    void setTask(CThreadExecutionTaskSPtr);

        
        //get this thread statistic
	TaskCycleStatPtr getStat();
	
    protected:
	friend class CThreadGroup;
	string threadIdentification;
    volatile bool m_stop;
	bool simulationMode;
    boost::chrono::microseconds waithTimeInMicrosecond;
    boost::chrono::nanoseconds computedWaithTimeInNanoseconds;
    boost::scoped_ptr<thread> m_thread;
	boost::mutex statMutex;
    bool firstLoop;
        //statistic data
	TaskCycleStatData statisticData;
	
        // Thread entry point
    void executeWork();   
    
    private:
        //thread group that contain this thread , if there is one
	CThreadGroup *parentCThreadGroup;
    CThreadExecutionTaskSPtr taskUnit;
    };
}
#endif

