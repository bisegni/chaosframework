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


#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#if BOOST_VERSION > 105300
//allocate the logger
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/attributes/attribute_name.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
#else
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/formatters.hpp>
namespace logging = boost::BOOST_LOG_NAMESPACE;
namespace fmt = boost::log::formatters;
#endif


#include "LogManager.h"
//
#define BASE_LOG_FORMAT         "[%TimeStamp%][%Severity%]: %_%"
#define EXTENDEND_LOG_FORMAT    "[%TimeStamp%][%Severity%][%ProcessID%][%ThreadID%]: %_%"

using namespace chaos;
using namespace chaos::common::log;

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
        case level::LSLNotice:
            out << "NOTICE";
            break;
        case level::LSLWarning:
            out << "WARNING";
            break;
        case level::LSLFatal:
            out << "FATAL";
            break;
    }
    return out;
}

void LogManager::init() throw(CException) {
    //get the log configuration
    level::LogSeverityLevel     logLevel                =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_LEVEL)?
    static_cast<level::LogSeverityLevel>(GlobalConfiguration::getInstance()->getConfiguration()->getInt32Value(InitOption::OPT_LOG_LEVEL)):
    level::LSLInfo;
    bool                        logOnConsole            =	GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_ON_CONSOLE)?
    GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_CONSOLE):false;
    bool                        logOnFile               =	GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_ON_CONSOLE)?
    GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_FILE):false;
    string                      logFileName             =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_FILE)?
    GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_LOG_FILE):"";
    uint32_t                    log_file_max_size_mb    =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_MAX_SIZE_MB)?GlobalConfiguration::getInstance()->getConfiguration()->getUInt32Value(InitOption::OPT_LOG_MAX_SIZE_MB):1;
    
    logging::add_common_attributes();
    boost::shared_ptr< logging::core > logger = boost::log::core::get();
    // Create a backend
#if (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 54
    logging::register_simple_formatter_factory< level::LogSeverityLevel, char  >("Severity");
    logger->set_filter(expr::attr< level::LogSeverityLevel >("Severity") >= logLevel);
#else
    logging::register_simple_formatter_factory< level::LogSeverityLevel  >("Severity");
    logger->set_filter(logging::filters::attr< level::LogSeverityLevel >("Severity") >= logLevel);
#endif
    
    
    if(logOnConsole){
#if (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 54
        logging::add_console_log(std::clog, logging::keywords::format = EXTENDEND_LOG_FORMAT);
#else
        logging::init_log_to_console(std::clog, logging::keywords::format = EXTENDEND_LOG_FORMAT);
#endif
    }
    
    if(logOnFile){
#if (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 54
        logging::add_file_log(keywords::file_name = logFileName,                                                                    // file name pattern
                              keywords::rotation_size = log_file_max_size_mb * 1024 * 1024,                                         // rotate files every 10 MiB...
                              keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),                // ...or at midnight
                              keywords::format = EXTENDEND_LOG_FORMAT,
                              keywords::auto_flush=true);
#else
        logging::init_log_to_file(logging::keywords::file_name = logFileName,                                                       // file name pattern
                                  logging::keywords::rotation_size = log_file_max_size_mb * 1024 * 1024,                            // rotate files every 10 MiB...
                                  logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),   // ...or at midnight
                                  logging::keywords::format = EXTENDEND_LOG_FORMAT,
                                  logging::keywords::auto_flush=true);
#endif
        
    }
    
    //enable the log in case of needs
    logger->set_logging_enabled(logOnConsole || logOnFile);
}
