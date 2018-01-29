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

#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/log/ChaosLoggingBackend.h>
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;



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
    level::LogSeverityLevel     logLevel                =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_LEVEL)?static_cast<level::LogSeverityLevel>(GlobalConfiguration::getInstance()->getConfiguration()->getInt32Value(InitOption::OPT_LOG_LEVEL)):level::LSLInfo;
    bool                        logOnConsole            =	GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_ON_CONSOLE)?GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_CONSOLE):false;
    bool                        logOnFile               =	GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_ON_CONSOLE)?GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_FILE):false;
    bool                        logOnSyslog             =	GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_ON_SYSLOG)?GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_LOG_ON_SYSLOG):false;
    string                      logFileName             =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_FILE)?GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_LOG_FILE):"";
    string                      logSyslogSrv            =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_SYSLOG_SERVER)?GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_LOG_SYSLOG_SERVER):"localhost";
    int                         logSyslogSrvPort        =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_SYSLOG_SERVER_PORT)?GlobalConfiguration::getInstance()->getConfiguration()->getInt32Value(InitOption::OPT_LOG_SYSLOG_SERVER_PORT):514;
    uint32_t                    log_file_max_size_mb    =   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_MAX_SIZE_MB)?GlobalConfiguration::getInstance()->getConfiguration()->getUInt32Value(InitOption::OPT_LOG_MAX_SIZE_MB):1;
    
    logging::add_common_attributes();
    boost::shared_ptr< logging::core > logger = boost::log::core::get();
    // Create a backend
    logging::register_simple_formatter_factory< level::LogSeverityLevel, char  >("Severity");
    logger->set_filter(expr::attr< level::LogSeverityLevel >("Severity") >= logLevel);
    if(logOnConsole){
        logging::add_console_log(std::clog, logging::keywords::format = EXTENDEND_LOG_FORMAT);
    }
    
    if(logOnFile){
        logging::add_file_log(keywords::file_name = logFileName,                                                                    // file name pattern
                              keywords::rotation_size = log_file_max_size_mb * 1024 * 1024,                                         // rotate files every 10 MiB...
                              keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),                // ...or at midnight
                              keywords::format = EXTENDEND_LOG_FORMAT,
                              keywords::auto_flush=true);
        
        
    }
    
    if(logOnSyslog) {
        // Creating a syslog sink.
        boost::shared_ptr< sinks::synchronous_sink< sinks::syslog_backend > > sink(new sinks::synchronous_sink< sinks::syslog_backend >
                                                                                   (
                                                                                    //keywords::facility = sinks::syslog::user,
                                                                                    keywords::use_impl = sinks::syslog::udp_socket_based,
                                                                                    keywords::format = EXTENDEND_LOG_FORMAT
                                                                                    )
                                                                                   );
        // Setting the remote address to sent syslog messages to.
        sink->locked_backend()->set_target_address(logSyslogSrv, logSyslogSrvPort);
        // Adding the sink to the core.b
        logger->add_sink(sink);
    }
    
    //enable the log in case of needs
    logger->set_logging_enabled(logOnConsole || logOnFile || logOnSyslog);
}

void LogManager::addMDSLoggingBackend(const std::string& source) {
    typedef sinks::synchronous_sink< ChaosLoggingBackend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t());
    sink->locked_backend()->setSource(source);
    boost::log::core::get()->add_sink(sink);
}
