    //
    //  ChaosUIToolkit.h
    //  UIToolkit
    //
    //  Created by bisegni on 08/09/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_ChaosUIToolkit_h
#define ChaosFramework_ChaosUIToolkit_h

#include <chaos/common/global.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/ChaosCommon.h>

#include <chaos/ui_toolkit/Common/DeviceDatasetCache.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLInfrastructureApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLRpcApi.h>

namespace chaos{
    //! Default chaos namespace used to group all ui api
    namespace ui{
        using namespace std;
  
        /*! \page page_cuit The Chaos User Interface Toolkit 
         *  \section page_cuit_sec Why an User Interface Toolkit
         *  
         */
        
        //! ChaosUIToolkit class is the main singleton needed to use UI chaos API
        /*! 
         Every developer that need to access chaos resource must use this pachecke to
         allocate and instantiate other api pachage
         */
        class ChaosUIToolkit : public ChaosCommon<ChaosUIToolkit> {
            friend class Singleton<ChaosUIToolkit>;
            /** \brief Client unique id */
            string clientInstanceUUID;
            /** \brief High level API for accessign device data*/
            HLDataApi *hlDataApi;
            /** \brief High level Api for RPC*/
            HLRpcApi * hlRpcApi;
            /** \brief High level Api for accessing infrastructure information*/
            HLInfrastructureApi * hlInfrastructureApi;
            /** \brief A cached shared across more UIToolkit instance on the same machine*/
            DeviceDatasetCache *globalDatasetCache;
            /** \brief point to the class the need to be allert when this UIToolkit must exit*/
            ServerDelegator *serverDelegator;    
        
                //!Constructor
            ChaosUIToolkit();
                //!Destructor
            ~ChaosUIToolkit();
        public:
                //!Initializaiton Method
            /*! 
                This method provide at the all API package initialization it must be called only at applciaiton startup
                \param argc the argc of main cpp program function
                \param argv the argv of main cpp program function
                \exception CException instance is created when something goes wrong
             */
            void init(int argc = 1, const char* argv[] = NULL) throw(CException);
            
                //!Deinitializaiton Method
            /*!
                this method provide all the resource deallocation
             \exception CException instance is created when something goes wrong
             */
            void deinit() throw(CException);
                //!High level RPC api accessor
            /*!
             \return The unique instance for the High level RPC API
             */
            HLRpcApi *getHLRpcApi() {return hlRpcApi;}
                //!High level Data api accessor
            /*!
             \return The unique instance for the High level Data API
             */
            HLDataApi *getHLDataApi() {return hlDataApi;}
                //!High level Infrastructure api accessor
            /*!
             \return The unique instance for the High level Infrastructure API
             */
            HLInfrastructureApi *getHLInfrastructureApi() {return hlInfrastructureApi;}
        };
    }
}
#endif
