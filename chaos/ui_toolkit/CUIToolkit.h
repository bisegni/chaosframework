    //
    //  CUIToolkit.h
    //  UIToolkit
    //
    //  Created by bisegni on 08/09/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosLib_CUIToolkit_h
#define ChaosLib_CUIToolkit_h

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
    namespace ui{
        using namespace std;
        /*
         Base UITookit class this is the main class that permit the access to all subapi class
         */
        class CUIToolkit : public ChaosCommon, public Singleton<CUIToolkit> {
            friend class Singleton<CUIToolkit>;
            string clientInstanceUUID;
            //api instance
            HLDataApi *hlDataApi;
            HLRpcApi * hlRpcApi;
            HLInfrastructureApi * hlInfrastructureApi;
            DeviceDatasetCache *globalDatasetCache;
            
            ServerDelegator *serverDelegator;    
        
            CUIToolkit();
            ~CUIToolkit();
        public:
            void init(int argc = 1, const char* argv[] = NULL) throw(CException);
            void deinit() throw(CException);
            HLRpcApi *getHLRpcApi() {return hlRpcApi;}
            HLDataApi *getHLDataApi() {return hlDataApi;}
            HLInfrastructureApi *getHLInfrastructureApi() {return hlInfrastructureApi;}
        };
    }
}
#endif
