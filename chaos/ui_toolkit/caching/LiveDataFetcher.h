//
//  LiveDataFetcher.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 1/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__LiveDataFetcher__
#define __CHAOSFramework__LiveDataFetcher__

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>

namespace chaos {
    
    class IODataDriver;
    
    namespace ui {
        namespace chaching {
            
            /*!
             Class that implemnt the CDatawrapper fetcher from !CHOAS live data.
             */
            class LiveDataFetcher : public chaos::caching_system::DataFetcherInterface<CDataWrapper>, public chaos::Configurable, public chaos::utility::StartableService {
                auto_ptr<char> currentRawDataPtr;
                
                chaos::IODataDriver *dataDriver;
                
                void getData(CDataWrapper& newData, uint64_t& ts);
                
            public:
                
                /*!
                 Default Constructor
                 \param _dataDriver the isntance ofthe driver to use with this fetcer
                 */
                LiveDataFetcher(chaos::IODataDriver *_dataDriver);
                
                /*!
                 Defaut Destructor
                 */
                virtual ~LiveDataFetcher();
                
                /*
                 Update the driver configuration
                 */
                CDataWrapper* updateConfiguration(CDataWrapper*);
                
                // Initialize instance
                void init(void*) throw(chaos::CException);
                
                // Start the implementation
                void start() throw(chaos::CException);
                
                // Deinit the implementation
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__LiveDataFetcher__) */
