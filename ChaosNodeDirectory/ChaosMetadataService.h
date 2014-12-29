/*
 *	ChaosMetadataService.h
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

#ifndef ChaosNodeDirectory_H
#define ChaosNodeDirectory_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>


namespace chaos {
    namespace nd {
            //! Chaos Node Directory base class
        /*!
         
         */
        class ChaosMetadataService :
		public ChaosCommon<ChaosMetadataService>,
		public ServerDelegator,
		public utility::StartableService {
            friend class Singleton<ChaosMetadataService>;
            
            static WaitSemaphore waitCloseSemaphore;
            
            ChaosMetadataService(){};
            ~ChaosMetadataService(){};
            static void signalHanlder(int);
            
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
            void start()throw(CException);
            void stop()throw(CException);
            void deinit()throw(CException);
        };
    }
}
    //#pragma GCC visibility pop
#endif
