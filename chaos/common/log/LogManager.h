/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__LogManager__
#define __CHAOSFramework__LogManager__

#include <iostream>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/Singleton.h>

namespace chaos {
    template<class T>
    class ChaosCommon;
    namespace common{
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
            
            class LogManager:
            public chaos::common::utility::Singleton<LogManager>{
                friend class Singleton<LogManager>;
                template<class T>
                friend class chaos::ChaosCommon;
                
                LogManager(){}
            public:
                void init() throw(CException);
            };
            
        }
    }
}
#endif /* defined(__CHAOSFramework__LogManager__) */
