/*
 *	LogManager.h
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

#ifndef __CHAOSFramework__LogManager__
#define __CHAOSFramework__LogManager__

#include <iostream>
#include <chaos/common/exception/exception.h>
namespace chaos {
    template<class T>
    class ChaosCommon;
    
    //! Define the level of the log
    namespace log {
        
        namespace level {
            
            typedef enum LogSeverityLevel {
                /// Enable the debugging information
                LSLDebug = 0,    // 1
                /// Normal debugging information
                LSLInfo  = 1,    // 2
                /// Normal debugging information
                LSLNotice  = 2,    // 2
                /// Log all error that occour but that don't prevent the killing of the toolkit
                LSLWarning = 3,    // 3
                /// Log the error the force the toolkit process to closs
                LSLFatal = 4      // 4
            } LogSeverityLevel;
        }
        
        class LogManager {
            template<class T>
            friend class chaos::ChaosCommon;
            
            LogManager(){}
        public:
            void init() throw(CException);
        };
        
        
    }
}
#endif /* defined(__CHAOSFramework__LogManager__) */
