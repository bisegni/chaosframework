/*
 *  ChaosCUToolkit.h
 *  ChaosFramework
 *
 *  Created by Claudio Bisegni on 13/02/11.
 *  Copyright 2011 INFN. All rights reserved.
 *
 */

/* The classes below are not exported */
#ifndef ChaosCUToolkit_H
#define ChaosCUToolkit_H
    //#pragma GCC visibility push(hidden)

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>


namespace chaos{
    /*! \page page_cut The Control Unit Toolkit
     *  \section page_cut_sec This toolkit represent the chaos driver for the real hardware to control
     *  
     */
    
    //! Chaos Contorl Unit Framework Master Class
    /*! 
     This class is a Singleton that need to be used to setup environment,
     add Custom Control unit, and start all Contro Unit environment
     */
    class ChaosCUToolkit : public ChaosCommon<ChaosCUToolkit>, public ServerDelegator {
        friend class Singleton<ChaosCUToolkit>;
        static boost::mutex monitor;
        static boost::condition endWaithCondition;
        
        ChaosCUToolkit(){};
        ~ChaosCUToolkit(){};
        static void signalHanlder(int);

    public:
        typedef boost::mutex::scoped_lock lock;
        void init(int argc = 1, const char* argv[] = NULL)  throw(CException);
        void start(bool waithUntilEnd=true, bool deinitiOnEnd=true);
        void stop();
        void deinit();
        void addControlUnit(AbstractControlUnit*);
};
}
    //#pragma GCC visibility pop
#endif