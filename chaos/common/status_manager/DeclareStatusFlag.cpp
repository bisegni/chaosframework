/*
 *	DeclareStatusFlag.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/status_manager/DeclareStatusFlag.h>

using namespace chaos::common::status_manager;

DeclareStatusFlag::DeclareStatusFlag(const std::string& _catalog_name):
StatusFlagCatalog(_catalog_name){}

DeclareStatusFlag::~DeclareStatusFlag(){}

void DeclareStatusFlag::addBoolFlag(const std::string flag_name,
                                    const std::string flag_description) {
    std::auto_ptr<StatusFlagBoolState> bool_flag(new StatusFlagBoolState(flag_name, flag_description));
    addFlag(bool_flag.release());
}

void DeclareStatusFlag::addLevelFlag(const std::string flag_name,
                                     const std::string flag_description,
                                     const MapFlagStateLevel& map_code_level_state) {
    std::auto_ptr<StatusFlag> level_flag(new StatusFlag(flag_name, flag_description));
    level_flag->addLevelsFromMap(map_code_level_state);
    addFlag(level_flag.release());
}
