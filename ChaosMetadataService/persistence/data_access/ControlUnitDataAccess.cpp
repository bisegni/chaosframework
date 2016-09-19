/*
 *	ControlUnitDataAccess.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#include "ControlUnitDataAccess.h"
using namespace chaos::metadata_service::persistence::data_access;

DEFINE_DA_NAME(ControlUnitDataAccess)

//! default constructor
ControlUnitDataAccess::ControlUnitDataAccess():
AbstractDataAccess("ControlUnitDataAccess") {
    
}

//!default destructor
ControlUnitDataAccess::~ControlUnitDataAccess() {
    
}

//!Remove all data befor the unit_ts timestamp
/*!
 \param control_unit_id the unique id of the control unit
 \param unit_ts is the timestamp befor wich we want to erase all data(included it)
 */
int ControlUnitDataAccess::eraseControlUnitDataBeforeTS(const std::string& control_unit_id,
                                                        uint64_t unit_ts) {
    return 0;
}
