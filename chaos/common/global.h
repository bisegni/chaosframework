/*	
 *	global.h
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
#ifndef ChaosFramework_global_h
#define ChaosFramework_global_h

/*! \mainpage !Chaos - Control system based on a Highly Abstracted and Open Structure
 *
 * \section intro_sec Introduction
 *
 * !CHOAS is a new idea on Control System software architecture, more information cam be found
 * on http://chaos.infn.it/
 *
 * \section install_sec Installation
 * Read the README.txt file in the root of source code folder
 */
#include <boost/version.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>


#include <chaos/common/endian.h>
#include <chaos/common/log/LogManager.h>

#if BOOST_VERSION > 105300
    //allocate the logger
    BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(chaosLogger, boost::log::sources::severity_logger_mt < chaos::log::level::LogSeverityLevel > )
#else 
    BOOST_LOG_DECLARE_GLOBAL_LOGGER(chaosLogger, boost::log::sources::severity_logger_mt < chaos::log::level::LogSeverityLevel > )
#endif 

#define LFTL_       BOOST_LOG_SEV(chaosLogger::get(), chaos::log::level::LSLFatal)
#define LDBG_       BOOST_LOG_SEV(chaosLogger::get(), chaos::log::level::LSLDebug)
#define LWRN_       BOOST_LOG_SEV(chaosLogger::get(), chaos::log::level::LSLWarning)
#define LERR_       BOOST_LOG_SEV(chaosLogger::get(), chaos::log::level::LSLNotice)
#define LAPP_       BOOST_LOG_SEV(chaosLogger::get(), chaos::log::level::LSLInfo)

    //define for chaos assert macro, it print the basiclay infromation to find
    //the error when the condition is not true
#ifndef DEBUG
#define CHAOS_ASSERT(x)
#define DEBUG_CODE(x)
#else
#define DEBUG_CODE(x) x
#define CHAOS_ASSERT(x) \
if (! (x)) \
{ \
cout << "ERROR!! Assert " << #x << " failed\n"; \
cout << " on line " << __LINE__  << "\n"; \
cout << " in file " << __FILE__ << "\n";  \
cout.flush();\
}
#endif


    //epoc for time stamp calculation
const boost::posix_time::ptime EPOCH(boost::gregorian::date(1970,1,1));

    //chaos assert to debug real badthing

#define CHAOS_UNUSED __attribute__((unused))

#include <chaos/common/version.h>

    //macro for decode the chaos exception
#define DECODE_CHAOS_EXCEPTION(x) \
LERR_ << "-----------Exception------------";\
LERR_ << "Domain:" << x.errorDomain;\
LERR_ << "Message:" << x.errorMessage;\
LERR_ << "Error Code;" << x.errorCode;\
LERR_ << "-----------Exception------------";

#define DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(dw, ex)\
dw->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ex.errorCode);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, ex.errorDomain);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, ex.errorMessage);

#define MANAGE_ERROR_IN_CDATAWRAPPERPTR(dw, code, message, domain)\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, domain);\
dw->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, code);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, message);

#define DELETE_OBJ_POINTER(x)\
delete(x); x=NULL;\


#define CHK_AND_DELETE_OBJ_POINTER(x)\
if (x){\
    delete(x); x=NULL;\
}

#define PRINT_LIB_HEADER  \
LAPP_ << "-----------------------------------------";\
LAPP_ << CSLIB_VERSION_HEADER;\
LAPP_ << CSLIB_VERSION_NUMBER;\
LAPP_ << "-----------------------------------------";

#define CHAOS_NOT_THROW(x)\
try { \
x \
} catch (...) { \
   \
}
/*
 Abstraction for the server delegator
 */
namespace chaos{
class ServerDelegator {
public:
    virtual ~ServerDelegator(){};
    virtual void stop() = 0;
};
}

#endif
