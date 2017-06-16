/*
 *	ChaosCUToolkit.h
 *	!CHAOS
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

#ifndef ChaosCUToolkit_H
#define ChaosCUToolkit_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include <chaos/cu_toolkit/control_manager/ControlManager.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <chaos/cu_toolkit/driver_manager/DriverManager.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            class ControlManager;
        }
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
            friend class common::utility::Singleton<ChaosCUToolkit>;
            friend class chaos::cu::control_manager::ControlManager;
            //static boost::mutex monitor;
            //static boost::condition endWaithCondition;
            
            static WaitSemaphore waitCloseSemaphore;
            
            ChaosCUToolkit();
            ~ChaosCUToolkit();
            static void signalHanlder(int);
        protected:
            void closeUIToolkit();
        public:
            typedef boost::mutex::scoped_lock lock;
            //! C and C++ attribute parser
            /*!
             Specialized option for startup c and cpp program main options parameter
             */
            void init(int argc, char* argv[]) throw (CException);
            //!stringbuffer parser
            /*
             specialized option for string stream buffer with boost semantics
             */
            void init(istringstream &initStringStream) throw (CException);
            void init(void *init_data)  throw(CException);
            void start() throw(CException);
            void stop()throw(CException);
            void deinit()throw(CException);
            
            void setProxyCreationHandler(chaos::cu::control_manager::ProxyLoadHandler load_handler);
            template<typename ControlUnitClass>
            void registerControlUnit() {
                control_manager::ControlManager::getInstance()->registerControlUnit<ControlUnitClass>();
            }
        };
    }
}
//#pragma GCC visibility pop
#endif
