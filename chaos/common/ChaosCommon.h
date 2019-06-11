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
#ifndef ChaosFramework_ChaosCommon_h
#define ChaosFramework_ChaosCommon_h

#include <chaos/common/global.h>
#include <chaos/common/log/LogManager.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/utility/InetUtility.h>

#include <chaos/common/plugin/PluginManager.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/action/DeclareAction.h>

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/async_central/AsyncCentralManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/version.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


//! Default chaos namespace used to group all common api
namespace chaos {
    struct __file_remover__ {
        const char*name;
        __file_remover__(const char*_name):name(_name){}
        ~__file_remover__(){std::remove(name);}
    };
    
    //! Chaos common engine class
    /*!
     This is the base class for the other toolkit, it thake care to initialize all common
     resource used for the base chaos function
     */
    class ChaosAbstractCommon:
    public common::utility::StartableService,
    public DeclareAction {
    protected:
        bool initialized,deinitialized;
        //!ingore unregistered program option
        bool ingore_unreg_po;
        
        //!return build infromation via rpc
       

        chaos::common::data::CDWUniquePtr _registrationAck(chaos::common::data::CDWUniquePtr data);

    public:
        //! Constructor Method
        /*!
         This method call the \ref GlobalConfiguration::preParseStartupParameters method, starting the
         allocation of the startup framework parameter
         */
        ChaosAbstractCommon();
        
        //! Destructor method
        /*!
         A more elaborate description of the destructor.
         */
        virtual ~ChaosAbstractCommon();
        
        /*!
         parse a config file before initializzation
         */
        void preparseConfigFile(std::istream &config_file_stream);
        
        /*!
         parse a config file before initializzation
         */
        void preparseCommandOption(int argc, const char* argv[]);
        
        //! C and C++ attribute parser
        /*!
         Specialized option for startup c and cpp program main options parameter
         */
        void init(int argc, const char* argv[]);
        
        //!stream parser
        /*
         specialized option for string stream buffer with boost semantics
         */
        void init(std::istream &initStream);
        
        //! Initialization methdo
        /*!
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        void init(void *init_data);
        void deinit();
                
        void start();
        void stop();
         /**
         * Return a JSON document with the version of the libraries
        */

        chaos::common::data::CDWUniquePtr getBuildInfo(chaos::common::data::CDWUniquePtr data);
        chaos::common::data::CDWUniquePtr getProcessInfo(chaos::common::data::CDWUniquePtr data);
        GlobalConfiguration* getGlobalConfigurationInstance();
    };
    
    //templated class to force singleton on master
    template<class T>
    class ChaosCommon:
    public common::utility::Singleton<T>,
    public ChaosAbstractCommon {
    protected:
        //! Constructor Method
        /*!
         This method call the \ref GlobalConfiguration::preParseStartupParameters method, starting the
         allocation of the startup framework parameter
         */
        ChaosCommon():
        ChaosAbstractCommon(){}
        
        //! Destructor method
        /*!
         A more elaborate description of the destructor.
         */
        virtual ~ChaosCommon(){}
    public:
        using ChaosAbstractCommon::init;
        using ChaosAbstractCommon::deinit;
        using ChaosAbstractCommon::start;
        using ChaosAbstractCommon::stop;
        using ChaosAbstractCommon::getGlobalConfigurationInstance;
       
    };
}
#endif
