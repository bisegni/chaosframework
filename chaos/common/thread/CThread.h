/*
 *	CThread.h
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
    typedef boost::shared_ptr<TaskCycleStatData> TaskCycleStatPtr;
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
    void setDelayBeetwenTask(uint64_t microsecond);
    
        //!Return the microsend of delay between a task and another
    uint64_t getDelayBeetwenTask();
    
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

