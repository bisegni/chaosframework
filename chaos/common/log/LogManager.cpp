/*
 *	LogManager.cpp
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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/formatters.hpp>

#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include "LogManager.h"

#define BASE_LOG_FORMAT         "[%TimeStamp%][%Severity%]: %_%"
#define EXTENDEND_LOG_FORMAT    "[%TimeStamp%][%Severity%][%ProcessID%][%ThreadID%]: %_%"

using namespace chaos;
using namespace chaos::log;
namespace logging = boost::BOOST_LOG_NAMESPACE;
namespace fmt = boost::log::formatters;

std::ostream& operator<<(std::ostream& out, const level::LogSeverityLevel& level )
{
    switch (level)
    {
        case level::LSLDebug:
            out << "DEBUG";
            break;
        case level::LSLInfo:
            out << "INFO";
            break;
        case level::LSLError:
            out << "ERROR";
            break;
        case level::LSLFatal:
            out << "FATAL";
            break;
    }
    return out;
}

void LogManager::init() throw(CException) {
    
    //get the log configuration
    level::LogSeverityLevel     logLevel        = static_cast<level::LogSeverityLevel>(GlobalConfiguration::getInstance()->getConfiguration()->getInt32Value(InitOption::OPT_LOG_LEVEL));
    bool                        logOnConsole    = GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_CONSOLE);
    bool                        logOnFile       = GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_FILE);
    string                      logFileName     = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_LOG_FILE);
    
    boost::log::add_common_attributes();
    
    boost::shared_ptr< logging::core > pCore = boost::log::core::get();
    
    logging::register_simple_formatter_factory< level::LogSeverityLevel  >("Severity");
    logging::core::get()->set_filter(logging::filters::attr< level::LogSeverityLevel >("Severity") >= logLevel);
    
    if(logOnConsole){
        logging::init_log_to_console(std::clog, logging::keywords::format = EXTENDEND_LOG_FORMAT);
    }
    
    if(logOnFile){
        logging::init_log_to_file(logging::keywords::file_name = logFileName,                  // file name pattern
                                  logging::keywords::rotation_size = 10 * 1024 * 1024,         // rotate files every 10 MiB...
                                  logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),// ...or at midnight
                                  logging::keywords::format = EXTENDEND_LOG_FORMAT,
                                  logging::keywords::auto_flush=true);
    }
    
    //enable the log in case of needs
    //boost::log::add_common_attributes();
    pCore->set_logging_enabled(logOnConsole || logOnFile);




 }
