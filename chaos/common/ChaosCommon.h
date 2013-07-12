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

#include <errno.h>
#include <sys/utsname.h>
#include <sstream>

#include <chaos/common/global.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/log/LogManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
//#include <chaos/common/data/cache/DataCache.h>

    //! Default chaos namespace used to group all common api
namespace chaos {
    using namespace std;
    using namespace boost;
    
        //! Chaos common engine class
    /*!
     This is the base class for the other toolkit, it thake care to initialize all common
     resource used for the base chaos function
     */
    template<class T>
    class ChaosCommon : public Singleton<T>{
        log::LogManager logManager;
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
        
    public:
            //! C and C++ attribute parser
        /*!
         Specialized option for startup c and cpp program main options parameter
         */
        void init(int argc, char* argv[]) throw (CException) {
            if(argv != NULL) {
                GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
            }
            init();
        }
            //!stringbuffer parser
        /*
         specialized option for string stream buffer with boost semantics
         */
        void init(istringstream &initStringStream) throw (CException) {
            GlobalConfiguration::getInstance()->parseStringStream(initStringStream);
            init();
        }
        
            //! Initialization methdo
        /*!
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        virtual void init() throw (CException) {
            int err = 0;
            struct utsname u_name;
            
            
                //startup logger
            logManager.init();
            
                //
            PRINT_LIB_HEADER
            
            err = uname(&u_name);
            if(err==-1){
                LAPP_ << "Platform: " << strerror(errno);
            } else {
                    LAPP_ << "Platform: " << u_name.sysname << " " << u_name.nodename << " " << u_name.release << " " << u_name.version << " " << u_name.machine;
            }
                
            LAPP_ << "Boost version: " << (BOOST_VERSION / 100000) << "."<< ((BOOST_VERSION / 100) % 1000)<< "."<< (BOOST_VERSION / 100000);
            LAPP_ << "Compiler Version: " << BOOST_COMPILER;
            LAPP_ << "-----------------------------------------";
                
                
                //find our ip
            string localIp;
            if(GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_PUBLISHING_IP)){
                localIp = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_PUBLISHING_IP);
            } else {
                InetUtility::scanForLocalNetworkAddress(localIp);
            }
            GlobalConfiguration::getInstance()->addLocalServerAddress(localIp.c_str());
                
            LAPP_ << "The local address chosen is:  " << GlobalConfiguration::getInstance()->getLocalServerAddress();
        }
                
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
