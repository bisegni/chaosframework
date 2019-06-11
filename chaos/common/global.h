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
#ifndef ChaosFramework_global_h
#define ChaosFramework_global_h

/*! \mainpage !Chaos - Control system based on a Highly Abstracted and Open Structure
 *
 * \section intro_sec Introduction
 *
 * !CHAOS is a new idea on Control System software architecture, more information cam be found
 * on http://chaos.infn.it/
 *
 * \section install_sec Installation
 * Read the README.txt file in the root of source code folder
 */


#include <chaos/common/endian.h>
#include <chaos/common/log/LogManager.h>

#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>



#include <cassert>

    //#include <chaos/common/debug/debug_new.h>

#include <stdlib.h>

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(chaosLogger, boost::log::sources::severity_logger_mt < chaos::common::log::level::LogSeverityLevel > )

#ifndef CSLIB_VERSION_MAJOR
#define CSLIB_VERSION_MAJOR "1"
#endif
#ifndef CSLIB_VERSION_MINOR
#define CSLIB_VERSION_MINOR "1"
#endif

#ifndef CSLIB_BUILD_ID
#define CSLIB_BUILD_ID 1
#endif
#define CHAOS_BOOST_LOCK_EXCEPTION_CACTH(exception_name, exception_code)\
catch(boost::exception_detail::error_info_injector<boost::io::too_many_args>& exception_name){\
exception_code\
}

#define CHAOS_BOOST_LOCK_WRAP_EXCEPTION(code, return_code)\
try{\
code\
}catch(boost::exception_detail::error_info_injector<boost::io::too_many_args>& lock_exception){\
return_code\
}

#define LERR_       BOOST_LOG_SEV(chaosLogger::get(), chaos::common::log::level::LSLFatal)
#define LDBG_       BOOST_LOG_SEV(chaosLogger::get(), chaos::common::log::level::LSLDebug)
#define LWRN_       BOOST_LOG_SEV(chaosLogger::get(), chaos::common::log::level::LSLWarning)
#define LNOTE_      BOOST_LOG_SEV(chaosLogger::get(), chaos::common::log::level::LSLNotice)
#define LAPP_       BOOST_LOG_SEV(chaosLogger::get(), chaos::common::log::level::LSLInfo)

#define DEFINE_LOG_HEADER(x) "[" #x "] - "
#define DEFINE_LOG_HEADER_1_P(x,y) "[" #x "-" << y << "] - "

#define INFO_LOG(x)         LAPP_   << DEFINE_LOG_HEADER(x)
#define INFO_LOG_1_P(x,y)   LAPP_   << DEFINE_LOG_HEADER_1_P(x,y)

#define NOTICE_LOG(x)       LNOTE_  << DEFINE_LOG_HEADER(x)
#define NOTICE_LOG_1_P(x,y) LNOTE_  << DEFINE_LOG_HEADER_1_P(x,y)

#define WARNING_LOG(x)      LWRN_   << DEFINE_LOG_HEADER(x)
#define WARNING_LOG_1_P(x,y)LWRN_   << DEFINE_LOG_HEADER_1_P(x,y)

#define DBG_LOG(x)          LDBG_   << DEFINE_LOG_HEADER(x) << __FUNCTION__ << " - "
#define DBG_LOG_1_P(x,y)    LDBG_   << DEFINE_LOG_HEADER_1_P(x,y) << __FUNCTION__ << " - "

#define ERR_LOG(x)          LERR_   << DEFINE_LOG_HEADER(x) << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "
#define ERR_LOG_1_P(x,y)    LERR_   << DEFINE_LOG_HEADER_1_P(x,y) << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

#define LOG_AND_TROW(log, num, msg)\
log << "("<<num<<") " << msg;\
throw chaos::CException(num, msg, __PRETTY_FUNCTION__);

#define LOG_AND_TROW_FORMATTED(log, num, f, p)\
try{\
CHAOS_BOOST_LOCK_WRAP_EXCEPTION( LOG_AND_TROW(log, num, boost::str(boost::format(f)p)), log << lock_exception.what();)\
}catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector< boost::io::bad_format_string> >& exc) {\
log<< exc.what();\
}

#define CHAOS_LASSERT_EXCEPTION(assertion, log, num, msg)\
if(!assertion) {LOG_AND_TROW(log, num, msg)}

#define CHECK_CDW_THROW_AND_LOG(cdw, log, num, msg)\
if(cdw.get() == NULL) {LOG_AND_TROW(log, num, msg)}

#define CHECK_KEY_THROW_AND_LOG(cdw, key, log, num, msg)\
if(cdw->hasKey(key) == false) {LOG_AND_TROW(log, num, msg)}

#define CHECK_CDW_THROW_AND_LOG_FORMATTED(cdw, log, num, frmt, param)\
if(cdw.() == NULL) {LOG_AND_TROW_FORMATTED(log, num, frmt, param)}

#define CHECK_KEY_THROW_AND_LOG_FORMATTED(cdw, key, log, num, frmt, param)\
if(cdw->hasKey(key) == false) {LOG_AND_TROW_FORMATTED(log, num, frmt, param)}

#define CHECK_ASSERTION_THROW_AND_LOG(assertion, log, num, msg)\
if(assertion == false) {LOG_AND_TROW(log, num, msg)}

#define CHECK_MANDATORY_KEY(d, k, el, en)\
CHECK_KEY_THROW_AND_LOG(d, k, el, en, CHAOS_FORMAT("%1% key is mandatory", %k));

#define CHECK_TYPE_OF_KEY(d, k, type, el, en)\
CHECK_ASSERTION_THROW_AND_LOG((d->is ## type ## Value(k)), el, en,\
CHAOS_FORMAT("%1% key need to be of type %2%", %k% #type));

    //define for chaos assert macro, it print the basiclay infromation to find
    //the error when the condition is not true
#ifndef DEBUG
#define CHAOS_ASSERT(x)
#define DEBUG_CODE(x)
#else
#define DEBUG_CODE(x) x
#define CHAOS_ASSERT(x) \
if (!(x)) \
{ \
std::cout << "ERROR!! Assert " << #x << " failed\n";	\
std::cout << " on line " << __LINE__  << "\n";	\
std::cout << " in file " << __FILE__ << "\n";	\
std::cout.flush();					\
}\
assert(x);
#endif

#define CHAOS_EXCEPTION(e,msg) \
{std::stringstream ss;\
ss<<__FILE__<<":"<<__LINE__<<":"<<__FUNCTION__;\
LDBG_<<"throwing exception in:"<<ss.str();\
LDBG_<<"message:"<<msg;\
throw chaos::CException(e,msg,ss.str());}

#define CHAOS_SET_PRECISION_TO_DOUBLE(number, factor)\
((double)((int)(number*factor))/factor)

    //epoc for time stamp calculation
const boost::posix_time::ptime EPOCH(boost::gregorian::date(1970,1,1));

    //chaos assert to debug real badthing
#ifdef _WIN32
#define CHAOS_UNUSED 
#else
#define CHAOS_UNUSED __attribute__((unused))
#endif
#include <chaos/common/version.h>

    //macro for decode the chaos exception
#define DECODE_CHAOS_EXCEPTION(x) \
LERR_ << "-----------Exception------------";\
LERR_ << "Domain:" << x.errorDomain;\
LERR_ << "Message:" << x.errorMessage;\
LERR_ << "Error Code;" << x.errorCode;\
LERR_ << "-----------Exception------------";

#define DECODE_CHAOS_EXCEPTION_ON_LOG(l, x) \
l << "-----------Exception------------\n"\
<< "Domain:" << x.errorDomain << "\n"\
<< "Message:" << x.errorMessage << "\n"\
<< "Error Code;" << x.errorCode << "\n"\
<< "-----------Exception------------";

#define DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(dw, ex)\
dw->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ex.errorCode);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, ex.errorDomain);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, ex.errorMessage);

#define MANAGE_ERROR_IN_CDATAWRAPPERPTR(dw, code, message, domain)\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, domain);\
dw->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, code);\
dw->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, message);

#define DELETE_OBJ_POINTER(x)\
delete(x); x=NULL;

#define PRINT_LIB_HEADER  \
LAPP_ << "-----------------------------------------";\
LAPP_ << CSLIB_VERSION_HEADER;\
LAPP_ << CSLIB_VERSION_MAJOR<<"."<<CSLIB_VERSION_MINOR<<CSLIB_VERSION_NUMBER;\
 LAPP_ << "Build ID:"<<CSLIB_BUILD_ID;\
LAPP_ << "Build Date:"<<__DATE__ <<" " <<__TIME__;	\
LAPP_ << "-----------------------------------------";

#define CHAOS_NOT_THROW(x)\
try {x} catch (chaos::CException& ex) { \
DECODE_CHAOS_EXCEPTION(ex)\
} catch (...) {}

#define CHAOS_BOOST_LOCK_ERR(l, x)\
try{\
l\
}catch(...){\
x\
}

#define CHAOS_FORMAT(f,p)\
boost::str(boost::format(f)p)

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
