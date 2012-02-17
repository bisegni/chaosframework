/*	
 *	ChaosCommon.h
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
#ifndef ChaosFramework_ChaosCommon_h
#define ChaosFramework_ChaosCommon_h

#include <stdio.h>      


#include <chaos/common/global.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/InetUtility.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
    //! Default chaos namespace used to group all common api
namespace chaos {    
    using namespace boost;
    namespace logging = boost::BOOST_LOG_NAMESPACE;
    
        //! Chaos common engine class
    /*! 
     This is the base class for the other toolkit, it thake care to initialize all common
     resource used for the base chaos function
     */
    template<class T>
    class ChaosCommon: public Singleton<T>{
        
    protected:
            //! Constructor Method
        /*! 
         Thi method call the \ref GlobalConfiguration::preParseStartupParameters method, starting the 
         allocation of the startup framework parameter
         */
        ChaosCommon(){
            GlobalConfiguration::getInstance()->preParseStartupParameters();
        }
        
        
            //! Destructor method
        /*! 
         A more elaborate description of the destructor.
         */
        virtual ~ChaosCommon() {};
        
            //! Initialization methdo
        /*! 
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        virtual void init(int argc, char* argv[])  throw(CException) {
            try {
                
                if(argv != NULL){
                    GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
                }
                
                bool logOnConsole = GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(UserOption::OPT_LOG_ON_CONSOLE);
                bool logOnFile = GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(UserOption::OPT_LOG_ON_FILE);
                string logFileName = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(UserOption::OPT_LOG_FILE);
                
                
                    //logging::core::get()->set_filter (logging::flt::attr<logging::trivial::severity_level>("Severity") >= logging::trivial::info);
                if(logOnConsole){
                    logging::init_log_to_console(std::clog, logging::keywords::format = "[%TimeStamp%]: %_%");
                }
                
                if(logOnFile){
                    logging::init_log_to_file
                    (
                     logging::keywords::file_name = logFileName,                  // file name pattern
                     logging::keywords::rotation_size = 10 * 1024 * 1024,             // rotate files every 10 MiB...
                     // ...or at midnight
                     logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
                     logging::keywords::format = "[%TimeStamp%]: %_%"                 // log record format
                     );
                }
                    //the version constant are defined into version.h
                    //file generate to every compilation
                PRINT_LIB_HEADER
                
                    //find our ip
                string localIp;
                InetUtility::scanForLocalNetworkAddress(localIp);
                GlobalConfiguration::getInstance()->addLocalServerAddress(localIp.c_str());
                LAPP_ << "The local address chose is:  " << GlobalConfiguration::getInstance()->getLocalServerAddress();
                
            } catch (CException &e) {
                DECODE_CHAOS_EXCEPTION(e);
            }
            
        }
        
        
    public:
            //! Return the global configuration for the current singleton instance
        /*!
         \return the GlobalConfiguration pointer to global instance
         */
        GlobalConfiguration *getGlobalConfigurationInstance() {
            return GlobalConfiguration::getInstance();
        }
        
    }; 
}
#endif
