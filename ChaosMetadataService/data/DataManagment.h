//
//  DataManagment.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DataManagment__
#define __CHAOSFramework__DataManagment__

#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/utility/SetupStateManager.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
    namespace cnd {
        namespace data {
            
            //! DataManagment
            /*!
             
             */
            class DataManagment: public utility::StartableService, private SetupStateManager  {
                
				chaos_data::CUSchemaDB *deviceDB;
            public:
                
                DataManagment();
                
                ~DataManagment();
                
                //! Initialize instance
                void init(void *initData = NULL) throw(chaos::CException);
                
                //! Start the implementation
                void start() throw(chaos::CException);
                
                //! Start the implementation
                void stop() throw(chaos::CException);
                
                //! Deinit the implementation
                void deinit() throw(chaos::CException);
                
                /*!
                 
                 */
				chaos_data::CUSchemaDB *getDeviceDatabase();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DataManagment__) */
