//
//  DriverErogatorInterface.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 05/11/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DriverErogatorInterface_h
#define CHAOSFramework_DriverErogatorInterface_h

#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

namespace chaos {
	namespace cu {
		
		//forward declaration
		class AbstractControlUnit;
		
		namespace driver_manager {
			
			class DriverErogatorInterface {
			public:
				//! return the accessor by an index
				/*!
					The index parameter correspond to the order that the driver infromation are
					added by the unit implementation into the function AbstractControlUnit::unitDefineDriver.
				*/
				virtual driver::DriverAccessor *getAccessoInstanceByIndex(int idx) = 0;
			};
		}
	}
}
#endif
