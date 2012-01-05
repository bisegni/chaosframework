/*
 *  ChaosCUToolkit.h
 *  ControlSystemLib
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

/*!
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *  
 * etc...
 */

namespace chaos{
    class ChaosCUToolkit : public ChaosCommon, public ServerDelegator {
    private:
        static boost::mutex monitor;
        static boost::condition endWaithCondition;
        
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