// assert_util.h

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <iostream>
#include <typeinfo>
#include <string>

#include <chaos/common/bson/base/status.h> // NOTE: This is safe as utils depend on base
#include <chaos/common/bson/inline_decls.h>
#include <chaos/common/bson/platform/compiler.h>

namespace bson {

    enum CommonErrorCodes {
        OkCode = 0,
        DatabaseDifferCaseCode = 13297 ,  // uassert( 13297 )
        SendStaleConfigCode = 13388 ,     // uassert( 13388 )
        RecvStaleConfigCode = 9996,       // uassert( 9996 )
        PrepareConfigsFailedCode = 13104, // uassert( 13104 )
        NotMasterOrSecondaryCode = 13436, // uassert( 13436 )
        NotMasterNoSlaveOkCode = 13435,   // uassert( 13435 )
        NotMaster = 10107,                // uassert( 10107 )
    };

    class AssertionCount {
    public:
        AssertionCount();
        void rollover();
        void condrollover( int newValue );

        int regular;
        int warning;
        int msg;
        int user;
        int rollovers;
    };

    extern AssertionCount assertionCount;

    class BSONObjBuilder;

    struct ExceptionInfo {
        ExceptionInfo() : msg(""),code(-1) {}
        ExceptionInfo( const char * m , int c )
            : msg( m ) , code( c ) {
        }
        ExceptionInfo( const std::string& m , int c )
            : msg( m ) , code( c ) {
        }
        void append( BSONObjBuilder& b , const char * m = "$err" , const char * c = "code" ) const ;
        std::string toString() const;
        bool empty() const { return msg.empty(); }
        void reset(){ msg = ""; code=-1; }
        std::string msg;
        int code;
    };

    /** helper class that builds error strings.  lighter weight than a StringBuilder, albeit less flexible.
        NOINLINE_DECL used in the constructor implementations as we are assuming this is a cold code path when used.

        example:
          throw UserException(123, ErrorMsg("blah", num_val));
    */
    class ErrorMsg {
    public:
        ErrorMsg(const char *msg, char ch);
        ErrorMsg(const char *msg, unsigned val);
        operator std::string() const { return buf; }
    private:
        char buf[256];
    };

    class DBException;
    std::string causedBy( const DBException& e );
    std::string causedBy( const std::string& e );
    bool inShutdown();

    /** Most mongo exceptions inherit from this; this is commonly caught in most threads */
    class DBException : public std::exception {
    public:
        DBException( const ExceptionInfo& ei ) : _ei(ei) { traceIfNeeded(*this); }
        DBException( const char * msg , int code ) : _ei(msg,code) { traceIfNeeded(*this); }
        DBException( const std::string& msg , int code ) : _ei(msg,code) { traceIfNeeded(*this); }
        virtual ~DBException() throw() { }

        virtual const char* what() const throw() { return _ei.msg.c_str(); }
        virtual int getCode() const { return _ei.code; }
        virtual void appendPrefix( std::stringstream& ss ) const { }
        virtual void addContext( const std::string& str ) {
            _ei.msg = str + causedBy( _ei.msg );
        }

        // Utilities for the migration to Status objects
        static ErrorCodes::Error convertExceptionCode(int exCode);

        Status toStatus(const std::string& context) const {
            return Status(convertExceptionCode(getCode()), context + causedBy(*this));
        }
        Status toStatus() const {
            return Status(convertExceptionCode(getCode()), this->what());
        }

        // context when applicable. otherwise ""
        std::string _shard;

        virtual std::string toString() const;

        const ExceptionInfo& getInfo() const { return _ei; }
    private:
        static void traceIfNeeded( const DBException& e );
    public:
        static bool traceExceptions;

    protected:
        ExceptionInfo _ei;
    };

    class AssertionException : public DBException {
    public:

        AssertionException( const ExceptionInfo& ei ) : DBException(ei) {}
        AssertionException( const char * msg , int code ) : DBException(msg,code) {}
        AssertionException( const std::string& msg , int code ) : DBException(msg,code) {}

        virtual ~AssertionException() throw() { }

        virtual bool severe() const { return true; }
        virtual bool isUserAssertion() const { return false; }
    };

    /* UserExceptions are valid errors that a user can cause, like out of disk space or duplicate key */
    class UserException : public AssertionException {
    public:
        UserException(int c , const std::string& m) : AssertionException( m , c ) {}
        virtual bool severe() const { return false; }
        virtual bool isUserAssertion() const { return true; }
        virtual void appendPrefix( std::stringstream& ss ) const;
    };

    class MsgAssertionException : public AssertionException {
    public:
        MsgAssertionException( const ExceptionInfo& ei ) : AssertionException( ei ) {}
        MsgAssertionException(int c, const std::string& m) : AssertionException( m , c ) {}
        virtual bool severe() const { return false; }
        virtual void appendPrefix( std::stringstream& ss ) const;
    };

    BSON_MONGO_COMPILER_NORETURN void verifyFailed(const char *msg, const char *file, unsigned line);
    BSON_MONGO_COMPILER_NORETURN void invariantFailed(const char *msg, const char *file, unsigned line);
    void wasserted(const char *msg, const char *file, unsigned line);
    BSON_MONGO_COMPILER_NORETURN void fassertFailed( int msgid );
    BSON_MONGO_COMPILER_NORETURN void fassertFailedNoTrace( int msgid );
    BSON_MONGO_COMPILER_NORETURN void fassertFailedWithStatus(
            int msgid, const Status& status);

    /** a "user assertion".  throws UserAssertion.  logs.  typically used for errors that a user
        could cause, such as duplicate key, disk full, etc.
    */
    BSON_MONGO_COMPILER_NORETURN void uasserted(int msgid, const char *msg);
    BSON_MONGO_COMPILER_NORETURN void uasserted(int msgid , const std::string &msg);

    /** msgassert and massert are for errors that are internal but have a well defined error text
        std::string.  a stack trace is logged.
    */
    BSON_MONGO_COMPILER_NORETURN void msgassertedNoTrace(int msgid, const char *msg);
    BSON_MONGO_COMPILER_NORETURN void msgasserted(int msgid, const char *msg);
    BSON_MONGO_COMPILER_NORETURN void msgasserted(int msgid, const std::string &msg);

    /* convert various types of exceptions to strings */
    std::string causedBy( const char* e );
    std::string causedBy( const DBException& e );
    std::string causedBy( const std::exception& e );
    std::string causedBy( const std::string& e );
    std::string causedBy( const std::string* e );
    std::string causedBy( const Status& e );

    /** aborts on condition failure */
    inline void fassert(int msgid, bool testOK) {
        if (BSON_MONGO_unlikely(!testOK)) fassertFailed(msgid);
    }

    inline void fassert(int msgid, const Status& status) {
        if (BSON_MONGO_unlikely(!status.isOK())) {
            fassertFailedWithStatus(msgid, status);
        }
    }


    /* "user assert".  if asserts, user did something wrong, not our code */
#undef BSON_MONGO_uassert
#define BSON_MONGO_uassert(msgid, msg, expr) (void)( BSON_MONGO_likely(!!(expr)) || (::bson::uasserted(msgid, msg), 0) )

    inline void uassertStatusOK(const Status& status) {
        if (BSON_MONGO_unlikely(!status.isOK())) {
            uasserted((status.location() != 0 ? status.location() : status.code()),
                      status.reason());
        }
    }

    /* warning only - keeps going */
#undef BSON_MONGO_wassert
#define BSON_MONGO_wassert(_Expression) (void)( BSON_MONGO_likely(!!(_Expression)) || (::bson::wasserted(#_Expression, __FILE__, __LINE__), 0) )

    /* display a message, no context, and throw assertionexception

       easy way to throw an exception and log something without our stack trace
       display happening.
    */
#undef BSON_MONGO_massert
#define BSON_MONGO_massert(msgid, msg, expr) (void)( BSON_MONGO_likely(!!(expr)) || (::bson::msgasserted(msgid, msg), 0) )
    /* same as massert except no msgid */
#undef BSON_MONGO_verify
#define BSON_MONGO_verify(_Expression) (void)( BSON_MONGO_likely(!!(_Expression)) || (::bson::verifyFailed(#_Expression, __FILE__, __LINE__), 0) )

#undef BSON_MONGO_invariant
#define BSON_MONGO_invariant(_Expression) (void)( BSON_MONGO_likely(!!(_Expression)) || (::bson::invariantFailed(#_Expression, __FILE__, __LINE__), 0) )

    /* dassert is 'debug assert' -- might want to turn off for production as these
       could be slow.
    */
#if defined(_DEBUG)
# define BSON_MONGO_dassert(x) fassert(16199, (x))
#else
# define BSON_MONGO_dassert(x)
#endif


# define dassert BSON_MONGO_dassert
# define verify BSON_MONGO_verify
# define invariant BSON_MONGO_invariant
# define uassert BSON_MONGO_uassert
# define wassert BSON_MONGO_wassert
# define massert BSON_MONGO_massert


    // some special ids that we want to duplicate

    // > 10000 asserts
    // < 10000 UserException

    enum { ASSERT_ID_DUPKEY = 11000 };

    /* throws a uassertion with an appropriate msg */
    BSON_MONGO_COMPILER_NORETURN void streamNotGood( int code, const std::string& msg, std::ios& myios );

    inline void assertStreamGood(unsigned msgid, const std::string& msg, std::ios& myios) {
        if( !myios.good() ) streamNotGood(msgid, msg, myios);
    }

    std::string demangleName( const std::type_info& typeinfo );

} // namespace bson

#undef BSON_MONGO_ASSERT_ON_EXCEPTION
#define BSON_MONGO_ASSERT_ON_EXCEPTION( expression ) \
    try { \
        expression; \
    } catch ( const std::exception &e ) { \
        stringstream ss; \
        ss << "caught exception: " << e.what() << ' ' << __FILE__ << ' ' << __LINE__; \
        msgasserted( 13294 , ss.str() ); \
    } catch ( ... ) { \
        massert( 10437 ,  "unknown exception" , false ); \
    }
#undef BSON_MONGO_ASSERT_ON_EXCEPTION_WITH_MSG
#define BSON_MONGO_ASSERT_ON_EXCEPTION_WITH_MSG( expression, msg ) \
    try { \
        expression; \
    } catch ( const std::exception &e ) { \
        stringstream ss; \
        ss << msg << " caught exception exception: " << e.what();   \
        msgasserted( 14043 , ss.str() );        \
    } catch ( ... ) { \
        msgasserted( 14044 , std::string("unknown exception") + msg );   \
    }

#define DESTRUCTOR_GUARD BSON_MONGO_DESTRUCTOR_GUARD
#undef BSON_MONGO_DESTRUCTOR_GUARD
#define BSON_MONGO_DESTRUCTOR_GUARD( expression ) \
    try { \
        expression; \
    } catch ( const std::exception &e ) { \
        problem() << "caught exception (" << e.what() << ") in destructor (" << __FUNCTION__ \
                  << ")" << std::endl; \
    } catch ( ... ) { \
        problem() << "caught unknown exception in destructor (" << __FUNCTION__ << ")" \
                  << std::endl; \
    }

