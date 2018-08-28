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

namespace chaos_io = chaos::common::io;
namespace chaos_data = chaos::common::data;

namespace chaos {
    namespace common{
		namespace io {
			class IODataDriver;
		}
	}
	
    namespace ui {
        namespace chaching {
            
            /*!
             Class that implemnt the CDatawrapper fetcher from !CHAOS live data.
             */
            class LiveDataFetcher :
			public chaos::caching_system::DataFetcherInterface<chaos_data::CDataWrapper>,
			public chaos::Configurable,
			public common::utility::StartableService {
                ChaosUniquePtr<char> currentRawDataPtr;
                
                chaos_io::IODataDriver *dataDriver;
                
                void getData(const std::string& key, chaos_data::CDataWrapper& newData, uint64_t& ts);
                
            public:
                
                /*!
                 Default Constructor
                 \param _dataDriver the isntance ofthe driver to use with this fetcer
                 */
                LiveDataFetcher(chaos_io::IODataDriver *_dataDriver);
                
                /*!
                 Defaut Destructor
                 */
                virtual ~LiveDataFetcher();
                
                /*
                 Update the driver configuration
                 */
                chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
                
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
